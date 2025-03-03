use cozy_chess::{
    get_bishop_moves, get_king_moves, get_knight_moves, get_pawn_attacks, get_pawn_quiets,
    get_rook_moves, BitBoard, Board, Color, File, Piece, Rank,
};

#[derive(Debug)]
#[repr(C)]
pub struct Features {
    pawn_pst: [f32; 48],
    knight_rank: [f32; 8],
    knight_file: [f32; 8],
    bishop_rank: [f32; 8],
    bishop_file: [f32; 8],
    rook_rank: [f32; 8],
    rook_file: [f32; 8],
    queen_rank: [f32; 8],
    queen_file: [f32; 8],
    king_rank: [f32; 8],
    king_file: [f32; 8],
    bishop_pair: f32,
    tempo: f32,
    isolated_pawn: f32,
    protected_pawn: f32,
    rook_on_open_file: f32,
    rook_on_semiopen_file: f32,
    shield_pawns: [f32; 4],
    king_on_open_file: f32,
    king_on_semiopen_file: f32,
    forward_mobility: [f32; 6],
    backward_mobility: [f32; 6],
    passed_pawn_ranks: [f32; 6],
    passer_own_king_dist: [f32; 8],
    passer_enemy_king_dist: [f32; 8],
    phalanx_pawn_rank: [f32; 6],
    king_attack_weight: [[f32; 5]; Color::NUM],
}

impl Features {
    pub const COUNT: usize = std::mem::size_of::<Self>() / std::mem::size_of::<f32>();

    fn rank(&mut self, piece: Piece) -> &mut [f32; 8] {
        match piece {
            Piece::Knight => &mut self.knight_rank,
            Piece::Bishop => &mut self.bishop_rank,
            Piece::Rook => &mut self.rook_rank,
            Piece::Queen => &mut self.queen_rank,
            Piece::King => &mut self.king_rank,
            _ => unreachable!(),
        }
    }

    fn file(&mut self, piece: Piece) -> &mut [f32; 8] {
        match piece {
            Piece::Knight => &mut self.knight_file,
            Piece::Bishop => &mut self.bishop_file,
            Piece::Rook => &mut self.rook_file,
            Piece::Queen => &mut self.queen_file,
            Piece::King => &mut self.king_file,
            _ => unreachable!(),
        }
    }

    pub fn extract(&mut self, board: &Board) {
        for &piece in &Piece::ALL {
            for unflipped_square in board.pieces(piece) {
                let color = board.color_on(unflipped_square).unwrap();
                let (square, inc) = match color {
                    Color::White => (unflipped_square, 1.0),
                    Color::Black => (unflipped_square.flip_rank(), -1.0),
                };

                if piece == Piece::Rook {
                    let file = square.file().bitboard();
                    if board.pieces(Piece::Pawn).is_disjoint(file) {
                        self.rook_on_open_file += inc;
                    } else if board.colored_pieces(color, Piece::Pawn).is_disjoint(file) {
                        self.rook_on_semiopen_file += inc;
                    }
                }

                if piece == Piece::King {
                    let file = square.file().bitboard();
                    if board.pieces(Piece::Pawn).is_disjoint(file) {
                        self.king_on_open_file += inc;
                    } else if board.colored_pieces(color, Piece::Pawn).is_disjoint(file) {
                        self.king_on_semiopen_file += inc;
                    }
                }

                match piece {
                    Piece::Knight | Piece::Bishop | Piece::Rook | Piece::Queen | Piece::King => {
                        self.rank(piece)[square.rank() as usize] += inc;
                        self.file(piece)[square.file() as usize] += inc;
                    }
                    Piece::Pawn => {
                        self.pawn_pst[match board.king(color).file() > File::D {
                            true => square.flip_file() as usize - 8,
                            false => square as usize - 8,
                        }] += inc
                    }
                }

                let mob = match piece {
                    Piece::Pawn => {
                        get_pawn_quiets(unflipped_square, color, board.occupied())
                            | (get_pawn_attacks(unflipped_square, color) & board.colors(!color))
                    }
                    Piece::Knight => get_knight_moves(unflipped_square),
                    Piece::Bishop => get_bishop_moves(unflipped_square, board.occupied()),
                    Piece::Rook => get_rook_moves(unflipped_square, board.occupied()),
                    Piece::Queen => {
                        get_bishop_moves(unflipped_square, board.occupied())
                            | get_rook_moves(unflipped_square, board.occupied())
                    }
                    Piece::King => get_king_moves(unflipped_square),
                };
                let mob = mob - board.colors(color);

                let mut forward_mob = mob;
                for rank in Rank::ALL {
                    if rank.relative_to(color) <= square.rank() {
                        forward_mob -= rank.bitboard();
                    }
                }
                self.forward_mobility[piece as usize] += inc * forward_mob.len() as f32;
                self.backward_mobility[piece as usize] += inc * (mob - forward_mob).len() as f32;

                let king_ring_attacks = (get_king_moves(board.king(!color)) & mob).len();
                if piece != Piece::King {
                    self.king_attack_weight[color as usize][piece as usize] += king_ring_attacks as f32;
                }
            }
        }

        for &color in &Color::ALL {
            for square in board.pieces(Piece::Pawn) & board.colors(color) {
                let mut passer_mask = square.file().adjacent() | square.file().bitboard();
                match color {
                    Color::White => {
                        for r in 0..=square.rank() as usize {
                            passer_mask &= !Rank::index(r).bitboard();
                        }
                    }
                    Color::Black => {
                        for r in square.rank() as usize..8 {
                            passer_mask &= !Rank::index(r).bitboard();
                        }
                    }
                }

                if !passer_mask.is_disjoint(board.colored_pieces(!color, Piece::Pawn)) {
                    continue;
                }

                if !(passer_mask & square.file().bitboard())
                    .is_disjoint(board.colored_pieces(color, Piece::Pawn))
                {
                    continue;
                }

                let (rank, inc) = match color {
                    Color::White => (square.rank() as usize, 1.0),
                    Color::Black => (square.rank().flip() as usize, -1.0),
                };
                self.passed_pawn_ranks[rank - 1] += inc;

                let king_dist = |king_color| {
                    let king = board.king(king_color);
                    let file_dist = (king.file() as u8).abs_diff(square.file() as u8);
                    let rank_dist = (king.rank() as u8).abs_diff(square.rank() as u8);
                    file_dist.max(rank_dist) as usize
                };
                self.passer_own_king_dist[king_dist(color)] += inc;
                self.passer_enemy_king_dist[king_dist(!color)] += inc;
            }
        }

        let pawns = board.colored_pieces(Color::White, Piece::Pawn);
        let pawn_attacks_right = BitBoard((pawns & !File::A.bitboard()).0 << 7);
        let pawn_attacks_left = BitBoard((pawns & !File::H.bitboard()).0 << 9);
        self.protected_pawn += ((pawn_attacks_left | pawn_attacks_right) & pawns).len() as f32;

        let pawns = board.colored_pieces(Color::Black, Piece::Pawn);
        let pawn_attacks_right = BitBoard((pawns & !File::A.bitboard()).0 >> 9);
        let pawn_attacks_left = BitBoard((pawns & !File::H.bitboard()).0 >> 7);
        self.protected_pawn -= ((pawn_attacks_left | pawn_attacks_right) & pawns).len() as f32;

        for color in Color::ALL {
            let inc = match color {
                Color::White => 1.0,
                Color::Black => -1.0,
            };
            if board.colored_pieces(color, Piece::Bishop).len() >= 2 {
                self.bishop_pair += inc;
            }
            if color == board.side_to_move() {
                self.tempo += inc;
            }

            for sq in board.colored_pieces(color, Piece::Pawn) {
                if sq
                    .file()
                    .adjacent()
                    .is_disjoint(board.colored_pieces(color, Piece::Pawn))
                {
                    self.isolated_pawn += inc;
                }
            }

            let phalanx_pawns = BitBoard(board.colored_pieces(color, Piece::Pawn).0 << 1)
                & board.colored_pieces(color, Piece::Pawn)
                & !File::A.bitboard();
            for phalanx in phalanx_pawns {
                self.phalanx_pawn_rank[phalanx.rank().relative_to(color) as usize - 1] += inc;
            }

            let king = board.king(color);
            if king.rank() == Rank::First.relative_to(color) {
                let pawns = board.colored_pieces(color, Piece::Pawn);
                let mut shield_pawns = 0;
                for dx in -1..=1 {
                    for dy in 1..3 {
                        if let Some(sq) = king.try_offset(dx, dy * inc as i8) {
                            if pawns.has(sq) {
                                shield_pawns += 1;
                                break;
                            }
                        }
                    }
                }
                self.shield_pawns[shield_pawns] += inc;
            }
        }
    }
}
