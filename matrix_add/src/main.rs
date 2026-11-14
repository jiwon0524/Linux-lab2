use std::io;

fn main() {
    let mut input = String::new();

    println!("행렬의 행(row) 개수를 입력하세요:");
    io::stdin().read_line(&mut input).unwrap();
    let rows: usize = input.trim().parse().unwrap();

    input.clear();
    println!("행렬의 열(column) 개수를 입력하세요:");
    io::stdin().read_line(&mut input).unwrap();
    let cols: usize = input.trim().parse().unwrap();

    let mut a: Vec<Vec<i32>> = vec![vec![0; cols]; rows];
    let mut b: Vec<Vec<i32>> = vec![vec![0; cols]; rows];
    let mut result: Vec<Vec<i32>> = vec![vec![0; cols]; rows];

    println!("첫 번째 행렬 A의 원소를 입력하세요.");

    for i in 0..rows {
        for j in 0..cols {
            input.clear();
            println!("A[{}][{}]:", i, j);
            io::stdin().read_line(&mut input).unwrap();
            a[i][j] = input.trim().parse().unwrap();
        }
    }

    println!("두 번째 행렬 B의 원소를 입력하세요.");

    for i in 0..rows {
        for j in 0..cols {
            input.clear();
            println!("B[{}][{}]:", i, j);
            io::stdin().read_line(&mut input).unwrap();
            b[i][j] = input.trim().parse().unwrap();
        }
    }

    for i in 0..rows {
        for j in 0..cols {
            result[i][j] = a[i][j] + b[i][j];
        }
    }

    println!("행렬 덧셈 결과:");

    for row in &result {
        for value in row {
            print!("{:4}", value);
        }
        println!();
    }
}
