#!/usr/bin/python3
import subprocess, sys, argparse, os.path, shutil, random, re, io

parser = argparse.ArgumentParser(
    prog="swiss.py",
    description="Swiss tournament runner",
)
parser.add_argument("engines", nargs="+")
parser.add_argument("--book", "-b", required=True, help="Path to opening book")
parser.add_argument("--baseline", help="Name of the engine to use as baseline for elo estimates")
parser.add_argument("--games", "-g", type=int, default=1, help="Number of game pairs per pairing")
parser.add_argument("--rounds", "-r", type=int, default=None, help="Number of rounds to play")
parser.add_argument("--match-concurrency", "-m", type=int, default=1, help="Number of matches to run in parallel")
parser.add_argument("--game-concurrency", "-c", type=int, default=1, help="Number of games within each match to run in parallel")
parser.add_argument("--time-control", "-t", default="1+0.01", help="Time control for games")
parser.add_argument("--random-seeds", action="store_true", help="Randomly seed instead of using argument order")
parser.add_argument(
    "--cutechess",
    default=shutil.which("cutechess-cli"),
    help="Path to cutechess-cli"
)
parser.add_argument(
    "--bayeselo",
    help="Path to bayeselo, or 'auto' to autodetect"
)
parser.add_argument(
    "--result-csv",
    default=os.devnull,
    help="Path to file to write CSV bayeselo results"
)

args = parser.parse_args()

error = False

if args.cutechess is None:
    print("Couldn't find cutechess-cli automatically")
    error = True

if args.bayeselo == "auto":
    args.bayeselo = shutil.which("bayeselo")
    if args.bayeselo is None:
        print("Couldn't find bayeselo automatically")
        error = True

if len(args.engines) % 2 != 0:
    print("There must be an even number of participants")
    error = True

if args.rounds is None:
    args.rounds = (len(args.engines)-1).bit_length()

if args.rounds > len(args.engines)-1:
    print("Number of rounds cannot exceed number of engines minus one")
    error = True

if args.random_seeds:
    random.shuffle(args.engines)

bookformat = args.book.rpartition(".")[2]
if bookformat not in ["pgn", "epd"]:
    print("book must be either in .pgn or .epd format")
    error = True

if error:
    sys.exit(1)

cutechess_command_common = [
    args.cutechess,
    "-each", "proto=uci", f"tc={args.time_control}", "dir=.",
    "-openings", f"file={args.book}", f"format={bookformat}", "order=random",
    "-repeat",
    "-games", "2",
    "-rounds", str(args.games),
    "-recover",
    "-concurrency", str(args.game_concurrency)
]

class Participant:
    def __init__(self, path, seed):
        self.cmd = path if path.startswith("/") or path.startswith(".") else "./" + path
        self.name = os.path.basename(path)
        self.score = 0
        self.seed = seed
        self.met = set()

    def __repr__(self):
        return f"Participant {self.__dict__}"

participants = [Participant(path, i) for i, path in enumerate(args.engines)]
baseline = None
for p in participants:
    if p.name == args.baseline:
        baseline = p.seed

def valid_pairings(p1, opponents):
    pairings = []
    for p2 in opponents:
        if p2.seed in p1.met:
            continue
        pairings.append((p1, p2))
    pairings.sort(key=lambda pair: (
        abs(pair[0].score - pair[1].score),
        -abs(pair[0].seed - pair[1].seed)
    ))
    return pairings

def round_pairings(participants):
    if len(participants) == 0:
        return []
    next_p = participants.pop()
    for p1, p2 in valid_pairings(next_p, participants):
        next_participants = [p for p in participants if p != p2]
        pairings = round_pairings(next_participants)
        if pairings is not None:
            pairings.append((p1, p2))
            return pairings

def handle_completion(current_processes, bayeselo):
    while True:
        pid, _ = os.wait()
        for i in range(len(current_processes)):
            proc, p1, p2 = current_processes[i]
            if proc.pid != pid:
                continue
            current_processes.pop(i)
            for line in proc.stdout.readlines():
                line = line.decode()
                if line.startswith("Score of"):
                    last_score_line = line
                if not line.startswith("..."):
                    continue

                natch = re.search(r"playing (White|Black): (\d+) - (\d+) - (\d+)", line)
                if natch is not None:
                    side, *outcome = natch.groups()
                    w, l, d = map(int, outcome)
                    if side == "White":
                        white, black = p1.seed, p2.seed
                    else:
                        white, black = p2.seed, p1.seed
                        w, l = l, w
                    for _ in range(w):
                        print(f"addresult {white} {black} 2", file=bayeselo)
                    for _ in range(l):
                        print(f"addresult {white} {black} 0", file=bayeselo)
                    for _ in range(d):
                        print(f"addresult {white} {black} 1", file=bayeselo)

            w, l, d = map(int, re.search(r"(\d+) - (\d+) - (\d+)", last_score_line).groups())
            print(f"Finished {p1.name} vs {p2.name}: {w} - {l} - {d}", flush=True)
            p1.score += w + d / 2
            p2.score += l + d / 2

            return

if args.bayeselo is None:
    bayeselo = open(os.devnull, "w")
else:
    bayeselo = io.StringIO()

print("prompt off", file=bayeselo)
for p in participants:
    print(f"addplayer {p.seed}", file=bayeselo)

for i in range(args.rounds):
    print(f"Starting Round {i+1} of {args.rounds}", flush=True)
    current_processes = []
    p = participants[:]
    p.sort(key=lambda p: (p.score, p.seed))
    for i, (p1, p2) in enumerate(round_pairings(p)):
        if len(current_processes) == args.match_concurrency:
            handle_completion(current_processes, bayeselo)

        print(f"Starting match {i+1} of {len(participants)//2} ({p1.name} vs {p2.name})", flush=True)
        p1.met.add(p2.seed)
        p2.met.add(p1.seed)
        proc = subprocess.Popen(
            cutechess_command_common + [
                "-engine", f"cmd={p1.cmd}", f"name={p1.seed}",
                "-engine", f"cmd={p2.cmd}", f"name={p2.seed}",
            ],
            stdout=subprocess.PIPE
        )
        current_processes.append((proc, p1, p2))

    while len(current_processes) > 0:
        handle_completion(current_processes, bayeselo)

    print(f"Finished round {i+1} of {args.rounds}.")
    print("Rank    Name            Score")
    ranked = sorted(participants, key=lambda p: (p.score, p.seed), reverse=True)
    for i, p in enumerate(ranked):
        print(f"{i+1:>4}    {p.name:<16}{p.score}")
    print(flush=True)

print("elo", file=bayeselo)
print("advantage 0", file=bayeselo)
print("mm", file=bayeselo)
if baseline is not None:
    print(f"offset 0 {baseline}", file=bayeselo)
print("ratings", file=bayeselo)

if args.bayeselo is not None:
    result = subprocess.run(args.bayeselo, input=bayeselo.getvalue(), text=True, capture_output=True)

    result_csv = open(args.result_csv, "w")

    print("Bayeselo Results:")
    print("Name                 Elo       +       -")
    print("Name,Elo,+,-", file=result_csv)
    process = False
    for line in result.stdout.splitlines():
        if process:
            result = re.match(r"\s*\d+\s+(\d+)\s+(-?\d+)\s+(\d+)\s+(\d+)", line)
            if result is not None:
                i, elo, plus, minus = map(int, result.groups())
                if i != baseline:
                    print(f"{participants[i].name:<16}{elo:>8}{plus:>8}{minus:>8}")
                    print(f"{participants[i].name},{elo},{plus},{minus}", file=result_csv)
        if line.startswith("Rank"):
            process = True

    result_csv.close()
