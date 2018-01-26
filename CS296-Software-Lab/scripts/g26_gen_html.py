#! /usr/bin/python3 
import re

fin = open('./doc/g26_project_report.tex','r')
fout = open('./doc/g26_project_report.html','w');
l = fout.write('<html>\n<head>\n<link href="http://fonts.googleapis.com/css?family=Open+Sans" rel="stylesheet" type="text/css">\n'+
				'<title>g26_prof_report</title>'+
				'<style>\n'+
										'body{margin-right:10em; margin-left:10em;font-family: "Open Sans", sans-serif;}'+
										'h1{text-align:center;color:#339933;text-transform:capitalize;}'+
										'.center{margin-right:auto; margin-left:auto;text-align:center;}'+
										'.subsection{font-size:25;text-align:center;color:#336633;text-transform:capitalize;}'+
				'</style></head>\n<body>\n');
line = fin.readline()
while(line):
	section = re.match(r'\\section\{Graph analysis\}',line);
	if(section):
		l = fout.write('<h1>Profiling Report for CS296 Project simulation</h1>');
		break;
	line = fin.readline()

while line:
	line = fin.readline();
	section = re.search(r'\\section',line);
	if section:
		break;
	if '\\subsection' in line:
		subsection = re.findall(r'\{([A-z0-9\ \,\.\/\:]*)\}',line);
		fout.write('</br><div class="subsection">'+ subsection[0] + '</div>' );
		continue;	
	if '\\begin' in line:
		if '{center}' in line:
			fout.write('<div class="center">');	
		elif '{equation}' in line or  '{lstlisting}' in line:
			fout.write('<pre>');		
		else:
			fout.write('<div>');
		continue;
	if '\\end' in line:
		if '{equation}' in line or  '{lstlisting}' in line:
			fout.write('</pre>');	
		else:	
			fout.write('</div>');
		continue;			
	
	if '\\includegraphics' in line:
		fout.write('<div>');
		image = re.findall(r'\{([A-z0-9\/]*)\}',line);
		image = image[0];
		strimage= image
		fout.write('<img src=\"' + strimage +'.png\" />');
		fout.write('</div>');
		continue;			
	comment = re.match(r'\%+',line);
	l = fout.write(line.replace("$","").replace("\\","").replace("`","\'"));
	
l = fout.write('</body>\n</html>');
fout.close()
fin.close()

