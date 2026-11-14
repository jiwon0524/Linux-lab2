# Linux Mini Shell

## 1. 프로젝트 개요

Linux 시스템 프로그래밍 실습을 위해 C 언어로 구현한 간단한 명령어 셸 프로그램이다.

fork(), execvp(), waitpid(), pipe(), dup2() 등의 시스템 호출을 활용하여 기본적인 Linux 셸 기능을 구현하였다.

## 2. 주요 기능

- 외부 명령어 실행
- 내장 명령어: cd, pwd, exit
- 백그라운드 프로세스 실행 (&)
- 시그널 처리 (SIGINT, SIGTSTP)
- 입력 리다이렉션 (<)
- 출력 리다이렉션 (>)
- 다중 파이프 (|)
- 작은따옴표 및 큰따옴표 처리
- 명령어 오류 및 예외 처리

## 3. 프로젝트 구조

```text
mini_shell/
├── include/
│   ├── builtin.h
│   ├── executor.h
│   ├── parser.h
│   ├── pipeline.h
│   ├── redirect.h
│   └── signals.h
├── src/
│   ├── builtin.c
│   ├── executor.c
│   ├── main.c
│   ├── parser.c
│   ├── pipeline.c
│   ├── redirect.c
│   └── signals.c
├── tests/
├── Makefile
└── README.md
```

## 4. 컴파일 및 실행

```bash
make
./mini_shell
```

컴파일 결과 삭제:

```bash
make clean
```

## 5. 실행 예시

```bash
mini-shell> pwd
mini-shell> ls -l
mini-shell> cd /tmp
mini-shell> echo Hello Linux
mini-shell> sleep 5 &
mini-shell> echo Hello > output.txt
mini-shell> cat < output.txt
mini-shell> ls | wc -l
mini-shell> cat output.txt | grep Linux | wc -w
mini-shell> exit
```

## 6. 주요 시스템 호출

| 시스템 호출 | 용도 |
|---|---|
| fork() | 자식 프로세스 생성 |
| execvp() | 외부 명령어 실행 |
| waitpid() | 자식 프로세스 상태 확인 |
| pipe() | 프로세스 간 데이터 전달 |
| dup2() | 표준 입출력 재지정 |
| open() | 파일 열기 |
| chdir() | 현재 작업 디렉터리 변경 |
| sigaction() | 시그널 동작 설정 |

## 7. 구현 범위 및 제한사항

- 최대 16개의 명령어를 파이프로 연결할 수 있다.
- 기본적인 따옴표 처리를 지원한다.
- 완전한 작업 제어(fg, bg)는 지원하지 않는다.
- cd 명령어의 ~ 경로 확장은 지원하지 않는다.
- 백슬래시 이스케이프 및 환경변수 확장은 지원하지 않는다.

## 8. 개발 환경

- OS: Ubuntu 24.04
- Language: C
- Compiler: GCC
- Build: Make
