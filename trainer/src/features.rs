use bytemuck::{Pod, Zeroable};
use cozy_chess::{get_bishop_moves, get_rook_moves, BitBoard, Board, Color, File, Piece, Rank};

#[derive(Debug, Clone, Copy, Zeroable, Pod)]
#[repr(C)]
pub struct Features {
    piece_rank: [[f32; 8]; 6],
}

impl Features {
    pub const COUNT: usize = std::mem::size_of::<Self>() / std::mem::size_of::<f32>();

    pub fn extract(&mut self, board: &Board) {
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
        }
    }
}
