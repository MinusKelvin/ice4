import torch, numpy, ctypes, json, subprocess, convert, os

from time import time, strftime

BATCH_SIZE = 16384
DATASET_PATH = "data/frzn-big3.bin"

subprocess.run(["cargo", "build", "--release", "-p", "trainer"])

PARSELIB = ctypes.cdll.LoadLibrary("../target/release/libtrainer.so")

PARSELIB.feature_count.argtypes = []
PARSELIB.feature_count.restype = ctypes.c_ulong
FEATURE_COUNT: int = PARSELIB.feature_count()

# PARSELIB.decode_data.argtypes = [
#     ctypes.POINTER(ctypes.c_ubyte),
#     ctypes.POINTER(ctypes.c_float),
#     ctypes.POINTER(ctypes.c_float),
#     ctypes.c_size_t,
# ]
PARSELIB.decode_data.restype = ctypes.c_bool

def batch_loader():
    with open(DATASET_PATH, "rb") as dataset:
        while True:
            data = dataset.read(BATCH_SIZE * 32)
            batch_size = len(data) // 32
            if batch_size == 0:
                return

            features = numpy.zeros((batch_size, FEATURE_COUNT), dtype=ctypes.c_float)
            phases = numpy.zeros((batch_size, 1), dtype=ctypes.c_float)
            targets = numpy.zeros((batch_size, 1), dtype=ctypes.c_float)
            features_c_array = numpy.ctypeslib.as_ctypes(features)
            phases_c_array = numpy.ctypeslib.as_ctypes(phases)
            targets_c_array = numpy.ctypeslib.as_ctypes(targets)

            result = PARSELIB.decode_data(
                data,
                features_c_array,
                phases_c_array,
                targets_c_array,
                batch_size
            )
            assert result

            yield torch.from_numpy(features), torch.from_numpy(phases), torch.from_numpy(targets)

class Model(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.mg = torch.nn.Linear(FEATURE_COUNT - 12, 1, bias=False)
        torch.nn.init.zeros_(self.mg.weight)
        self.eg = torch.nn.Linear(FEATURE_COUNT - 12, 1, bias=False)
        torch.nn.init.zeros_(self.eg.weight)
        self.king_attack = torch.nn.Linear(5, 1, bias=False)
        torch.nn.init.ones_(self.king_attack.weight)

    def forward(self, features, phase):
        linear = features[:, :-12]
        king_safety = features[:, -12:-2].reshape((-1, 2, 5))
        pawns = features[:, -2:].reshape((-1, 2))

        mg = self.mg(linear)
        eg = self.eg(linear)

        king_attack = self.king_attack(king_safety) ** 2
        king_attack = king_attack[:, 0] - king_attack[:, 1]

        stronger_side_pawns = torch.gather(pawns, -1, (eg < 0).long())
        eg_scale = (128 - (8 - stronger_side_pawns) ** 2) / 128

        score = torch.lerp(eg * eg_scale, mg + king_attack, phase)

        return torch.sigmoid(score)

train_id = strftime("%Y-%m-%d-%H-%M-%S-") + os.path.splitext(os.path.basename(DATASET_PATH))[0]
model = Model()
optimizer = torch.optim.Adam(model.parameters(), lr=0.01)

print(train_id)

for epoch in range(15):
    if epoch in [8, 12]:
        optimizer.param_groups[0]["lr"] /= 10

    iters = 0
    running_loss = 0
    poses = 0
    start = time()
    for features, phases, targets in batch_loader():
        optimizer.zero_grad()
        outputs = model(features, phases)
        loss = torch.mean(torch.abs(outputs - targets) ** 2.6)
        loss.backward()
        optimizer.step()

        running_loss += loss.item()
        iters += 1
        poses += features.shape[0]

    loss = running_loss / iters
    print(f"epoch {epoch+1: 2}:    loss {loss:.4g}    pos/s {round(poses / (time() - start))}")

data = {
    "train_id": train_id, "loss": loss, "params": {
        name: param.detach().cpu().numpy().tolist()
        for name, param in model.named_parameters()
    }
}
with open(f"models/{train_id}.json", "w") as json_file:
    json.dump(data, json_file)

convert.dump_result(data)
