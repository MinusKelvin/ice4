use bytemuck::{Pod, Zeroable};
use cozy_chess::{get_bishop_moves, get_rook_moves, BitBoard, Board, Color, File, Piece, Rank};

#[derive(Debug, Clone, Copy, Zeroable, Pod)]
#[repr(C)]
pub struct Features {
    material: [f32; 6],
}

impl Features {
    pub const COUNT: usize = std::mem::size_of::<Self>() / std::mem::size_of::<f32>();

    pub fn extract(&mut self, board: &Board) {
        for piece in Piece::ALL {
            self.material[piece as usize] = board.colored_pieces(Color::White, piece).len() as f32
                - board.colored_pieces(Color::Black, piece).len() as f32;
        }
    }
}
