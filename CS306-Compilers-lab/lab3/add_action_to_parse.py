import sys
if(len(sys.argv) < 2):
	exit()
tokens = []	
with open(sys.argv[1]) as file:
	for line in file:
		line_text = line.strip(' \t\n')
		print(line.strip('\n'))
		splitted = line_text.split()		
		if(len(splitted) > 0):
			if(splitted[0] == "%token"):
				tokens = tokens + splitted[1:]
			elif(splitted[0][:1].isalpha()):
				start_node = splitted[0]
			elif(splitted[0] == ":" or splitted[0] == "|"):
				print "\t{"
				print "\t\t $$=count++;"
				print '\t\t fout << \"N\" << $$ <<  \"[label = \\"'+ start_node +'\\"]\\n";'
				i = 0
				for word in splitted[1:]:
					i += 1
					if(word in tokens or word.startswith("\'")):
						print '\t\t fout << \"N\"<< count <<  \" [label = \\"'+ word +'\\"]\\n";'
						print "\t\t create_edge($$,count);\n" + "\t\t count++;"

					else:					
						print "\t\t create_edge($$,$"+`i`,");"

				print "\t}"	


	

