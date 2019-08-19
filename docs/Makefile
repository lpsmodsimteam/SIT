DOC ?= ssti.tex

.PHONY: convert convert1 convert2
convert: convert1 convert2
convert1 convert2:
	# convert LaTex file to PDF
	pdflatex ${DOC}

clean:
	# remove all temporary files
	rm -rf *.aux *.log *.out
