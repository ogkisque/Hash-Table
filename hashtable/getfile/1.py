def parse_line (line, a):
	seps = ".,!? :-[]%^;#&*/—"
	word = ""
	for i in line:
		if i in seps:
			if word != "":
				a.append(word.upper())
				word = ""
		else:
			word += i

a = []

for line in open ("text.txt"):
	parse_line (line, a)
	
fil = open ("newtext.txt", "w")
for i in a:
	fil.write (i + "\n")
	
fil.close()
