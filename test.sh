#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 0
assert 42 42
assert 50 "42 + 8"
assert 50 "42+7 -1+ 2 "
assert 23 "(20 + 3)"
assert 23 "((20 - 11) * 2 + 5)"
assert 23 "(20 + 26)/2"

echo OK
