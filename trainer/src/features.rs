use bytemuck::{Pod, Zeroable};
use cozy_chess::{
    get_bishop_moves, get_king_moves, get_knight_moves, get_pawn_attacks, get_pawn_quiets,
    get_rook_moves, BitBoard, Board, Color, File, Piece, Rank, Square,
};

#[derive(Debug, Clone, Copy, Zeroable, Pod)]
#[repr(C)]
pub struct Features {
    piece_rank: [[f32; 8]; 6],
    piece_file: [[f32; 8]; 6],
    mobility: [f32; 6],
    passed_pawn: [f32; 8],
    isolated_pawn: f32,
    doubled_pawn: f32,
    king_attacks: f32,
    double_king_attacks: f32,
    under_threat: [f32; 6],
    tempo: f32,
    rook_behind_pawn: f32,
    bishop_pair: f32,
    passer_attacked: f32,
    queen_attacking_king: f32,
}

impl Features {
    pub const COUNT: usize = std::mem::size_of::<Self>() / std::mem::size_of::<f32>();

    pub fn extract(&mut self, board: &Board) {
        let mut piece_attack_map = [[[0; 64]; 6]; 2];
        let mut count_attack_map = [[0; 64]; 2];
        for sq in board.occupied() {
            let color = board.color_on(sq).unwrap();
            let piece = board.piece_on(sq).unwrap();

            let mob = match piece {
                Piece::Pawn => {
                    get_pawn_quiets(sq, color, board.occupied())
                        | get_pawn_attacks(sq, color) & board.colors(!color)
                }
                Piece::Knight => get_knight_moves(sq),
                Piece::Bishop => get_bishop_moves(sq, board.occupied()),
                Piece::Rook => get_rook_moves(sq, board.occupied()),
                Piece::Queen => {
                    get_bishop_moves(sq, board.occupied()) | get_rook_moves(sq, board.occupied())
                }
                Piece::King => get_king_moves(sq),
            };
            let mob = mob & !board.colors(color);

            let attacks = match piece {
                Piece::Pawn => get_pawn_attacks(sq, color),
                _ => mob,
            };

            for sq in attacks {
                piece_attack_map[color as usize][piece as usize][sq as usize] += 1;
                count_attack_map[color as usize][sq as usize] += 1;
            }

            let inc = match color {
                Color::White => 1.0,
                Color::Black => -1.0,
            };
            self.mobility[piece as usize] += inc * mob.len() as f32;
        }

        let mut pawn_ahead = [BitBoard::EMPTY; 2];
        let mut pawn_behind = [BitBoard::EMPTY; 2];

        for pawn in board.pieces(Piece::Pawn) {
            let color = board.color_on(pawn).unwrap();
            let dir = match color {
                Color::White => 1,
                Color::Black => -1,
            };
            pawn_behind[color as usize] |= pawn.bitboard();
            for i in 1..8 {
                if let Some(sq) = pawn.try_offset(0, dir * i) {
                    pawn_ahead[color as usize] |= sq.bitboard();
                }
                if let Some(sq) = pawn.try_offset(0, -dir * i) {
                    pawn_behind[color as usize] |= sq.bitboard();
                }
            }
        }

        for sq in Square::ALL {
            if piece_attack_map[Color::White as usize][Piece::King as usize][sq as usize] > 0 {
                let attacks = count_attack_map[Color::Black as usize][sq as usize];
                self.king_attacks -= attacks as f32;
                self.double_king_attacks -= (attacks >= 2) as i32 as f32;
                if piece_attack_map[Color::Black as usize][Piece::Queen as usize][sq as usize] > 0 {
                    self.queen_attacking_king -= 1.0;
                }
            }
            if piece_attack_map[Color::Black as usize][Piece::King as usize][sq as usize] > 0 {
                let attacks = count_attack_map[Color::White as usize][sq as usize];
                self.king_attacks += attacks as f32;
                self.double_king_attacks += (attacks >= 2) as i32 as f32;
                if piece_attack_map[Color::White as usize][Piece::Queen as usize][sq as usize] > 0 {
                    self.queen_attacking_king += 1.0;
                }
            }
        }

        for unflipped_sq in board.occupied() {
            let color = board.color_on(unflipped_sq).unwrap();
            let piece = board.piece_on(unflipped_sq).unwrap();
            let inc = match color {
                Color::White => 1.0,
                Color::Black => -1.0,
            };
            let sq = match color {
                Color::White => unflipped_sq,
                Color::Black => unflipped_sq.flip_rank(),
            };

            if piece == Piece::Pawn
                && !(unflipped_sq
                    .try_offset(-1, 0)
                    .map_or(false, |sq| pawn_ahead[!color as usize].has(sq))
                    || pawn_ahead[!color as usize].has(unflipped_sq)
                    || unflipped_sq
                        .try_offset(1, 0)
                        .map_or(false, |sq| pawn_ahead[!color as usize].has(sq)))
            {
                self.passed_pawn[sq.rank() as usize] += inc;

                if count_attack_map[!color as usize][unflipped_sq as usize] > 0 {
                    self.passer_attacked += inc;
                }
            }

            if piece == Piece::Pawn
                && !(unflipped_sq.try_offset(-1, 0).map_or(false, |sq| {
                    pawn_ahead[color as usize].has(sq) || pawn_behind[color as usize].has(sq)
                }) || unflipped_sq.try_offset(1, 0).map_or(false, |sq| {
                    pawn_ahead[color as usize].has(sq) || pawn_behind[color as usize].has(sq)
                }))
            {
                self.isolated_pawn += inc;
            }

            if piece == Piece::Pawn && pawn_ahead[color as usize].has(unflipped_sq) {
                self.doubled_pawn += inc;
            }

            if piece == Piece::Rook && pawn_behind[color as usize].has(unflipped_sq) {
                self.rook_behind_pawn += inc;
            }

            if Piece::ALL.iter().any(|&p| {
                p < piece
                    && piece_attack_map[!color as usize][p as usize][unflipped_sq as usize] > 0
            }) {
                self.under_threat[piece as usize] += inc;
            }

            self.piece_rank[piece as usize][sq.rank() as usize] += inc;
            self.piece_file[piece as usize][sq.file() as usize] += inc;
        }

        self.tempo = match board.side_to_move() {
            Color::White => 1.0,
            Color::Black => -1.0,
        };

        self.bishop_pair = ((board.colored_pieces(Color::White, Piece::Bishop).len() >= 2) as i32
            - (board.colored_pieces(Color::Black, Piece::Bishop).len() >= 2) as i32)
            as f32;
    }
}
