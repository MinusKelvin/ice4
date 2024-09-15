use cozy_chess::{
    get_bishop_moves, get_king_moves, get_knight_moves, get_pawn_attacks, get_pawn_quiets,
    get_rook_moves, BitBoard, Board, Color, File, Piece, Rank, Square,
};

#[derive(Debug)]
#[repr(C)]
pub struct Features {
    pawn_pst: [f32; 48],
    king_pst: [f32; 16],
    knight_pst: [f32; 16],
    knight_quadrant: [f32; 3],
    bishop_pst: [f32; 16],
    bishop_quadrant: [f32; 3],
    rook_pst: [f32; 16],
    rook_quadrant: [f32; 3],
    queen_pst: [f32; 16],
    queen_quadrant: [f32; 3],
    bishop_pair: f32,
    tempo: f32,
    isolated_pawn: f32,
    protected_pawn: f32,
    rook_on_open_file: f32,
    rook_on_semiopen_file: f32,
    shield_pawns: [f32; 4],
    king_on_open_file: f32,
    king_on_semiopen_file: f32,
    mobility: [f32; 6],
    king_ring_attacks: f32,
    passed_pawn_ranks: [f32; 6],
    passer_own_king_dist: [f32; 8],
    passer_enemy_king_dist: [f32; 8],
}

impl Features {
    pub const COUNT: usize = std::mem::size_of::<Self>() / std::mem::size_of::<f32>();

    fn quad(&mut self, piece: Piece) -> &mut [f32; 3] {
        match piece {
            Piece::Knight => &mut self.knight_quadrant,
            Piece::Bishop => &mut self.bishop_quadrant,
            Piece::Rook => &mut self.rook_quadrant,
            Piece::Queen => &mut self.queen_quadrant,
            _ => unreachable!(),
        }
    }

    fn quad_pst(&mut self, piece: Piece) -> &mut [f32; 16] {
        match piece {
            Piece::Knight => &mut self.knight_pst,
            Piece::Bishop => &mut self.bishop_pst,
            Piece::Rook => &mut self.rook_pst,
            Piece::Queen => &mut self.queen_pst,
            _ => unreachable!(),
        }
    }

    pub fn extract(&mut self, board: &Board) {
        let mut kras = [0; 2];

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
                    Piece::Knight | Piece::Bishop | Piece::Rook | Piece::Queen => {
                        let quad = (square.file() > File::D) as usize * 2
                            + (square.rank() > Rank::Fourth) as usize;
                        if quad != 0 {
                            self.quad(piece)[quad - 1] += inc;
                        }
                        self.quad_pst(piece)[quadrant_feature(square)] += inc;
                    }
                    Piece::King => {
                        self.king_pst
                            [square.rank() as usize / 2 * 4 + square.file() as usize / 2] += inc
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
                kras[color as usize] += (get_king_moves(board.king(!color)) & mob).len();
                self.mobility[piece as usize] += inc * (mob & !board.colors(color)).len() as f32;
            }
        }

        self.king_ring_attacks =
            kras[Color::White as usize].pow(2) as f32 - kras[Color::Black as usize].pow(2) as f32;

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

fn quadrant_feature(square: Square) -> usize {
    let square = match square.file() > File::D {
        true => square.flip_file(),
        false => square,
    };
    let square = match square.rank() > Rank::Fourth {
        true => square.flip_rank(),
        false => square,
    };
    square.rank() as usize * 4 + square.file() as usize
}
