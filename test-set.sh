#! /bin/sh

set -e

printf '   '
printf ' | %5s' '  A  ' '  B  ' 'A u B' 'A n B' 'A \ B' 'B \ A' 'A-:-B'
echo

trap 'rm -f -- s.*' 0

EXIT=0
NUM=100
while [ $NUM -gt 0 ]; do
	NUM=`expr $NUM - 1 || true`
	for t in is bis; do

		./gen-set $t >s.A
		./${t}_expr print -f s.A >s.a
		./gen-set $t >s.B
		./${t}_expr print -f s.B >s.b
		W_a=`wc -w <s.A`
		W_b=`wc -w <s.B`

		./${t}_expr add  -f s.a -- -f s.b >s.aub
		./${t}_expr mul  -f s.a -- -f s.b >s.anb
		./${t}_expr sub  -f s.a -- -f s.b >s.a_b
		./${t}_expr sub  -f s.b -- -f s.a >s.b_a
		./${t}_expr diff -f s.a -- -f s.b >s.a__b

		for s in a b aub anb a_b b_a a__b; do
			eval C_$s='`wc -w <s.$s`'
			eval E_$s='" "'
		done
		E=;

		if [ $C_a   -gt $W_a ]; then E_a='*'; fi
		if [ $C_b   -gt $W_b ]; then E_b='*'; fi
		if [ $C_a_b -gt $C_a ] || [ $C_a_b -ne `expr $C_a - $C_anb` ]; then
			E_a_b='*'
		fi
		if [ $C_b_a -gt $C_b ] || [ $C_b_a -ne `expr $C_b - $C_anb` ]; then
			E_b_a='*'
		fi
		if [ $C_anb -gt $C_a ] || [ $C_anb -gt $C_b ]; then
			E_anb='*'
		fi
		if [ `expr \`expr $C_a + $C_b\` - $C_anb` -ne $C_aub ]; then
			E_aub='*'
		fi
		if [ `expr $C_a_b + $C_b_a` -ne $C_a__b ]; then E_a__b='*'; fi

		printf %3s $t
		printf ' |%s%5d' "$E_a" $C_a "$E_b" $C_b "$E_aub" $C_aub \
			"$E_anb" $C_anb "$E_a_b" $C_a_b "$E_b_a" $C_b_a "$E_a__b" $C_a__b

		case "$E_a$E_b$E_aub$E_anb$E_a_b$E_b_a$E_a__b" in
			(*\**) echo ' error' ; EXIT=1 ;;
			(*) echo ;;
		esac

		./${t}_expr sub  -f s.a -- -f s.anb >s.a_anb
		if ./${t}_expr '!=' -f s.a_b -- -f s.a_anb; then
			echo 'error: A \ B != A \ (A n B)'
			EXIT=1
		fi

		./${t}_expr sub  -f s.b -- -f s.anb >s.b_anb
		if ./${t}_expr '!=' -f s.b_a -- -f s.b_anb; then
			echo 'error: B \ A != B \ (A n B)'
			EXIT=1
		fi
	done
done

exit $EXIT
