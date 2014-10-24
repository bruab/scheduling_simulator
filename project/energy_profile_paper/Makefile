PAPER = paper


default: $(PAPER).pdf 


$(PAPER).pdf: $(PAPER).tex biblio.bib 

	pdflatex $(PAPER)
	bibtex $(PAPER)
	pdflatex $(PAPER)
	pdflatex $(PAPER)

clean:
	/bin/rm -f $(PAPER).aux $(PAPER).pdf $(PAPER).bbl $(PAPER).log $(PAPER).blg
