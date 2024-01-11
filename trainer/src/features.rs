use bytemuck::{Pod, Zeroable};
use cozy_chess::{
    get_bishop_moves, get_king_moves, get_knight_moves, get_pawn_attacks, get_pawn_quiets,
    get_rook_moves, BitBoard, Board, Color, File, Piece, Rank,
};

#[derive(Debug, Clone, Copy, Zeroable, Pod)]
#[repr(C)]
pub struct Features {
    piece_rank: [[f32; 8]; 6],
    piece_file: [[f32; 8]; 6],
    mobility: [f32; 6],
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

            self.piece_rank[piece as usize][sq.rank() as usize] += inc;
            self.piece_file[piece as usize][sq.file() as usize] += inc;
        }
    }
}
