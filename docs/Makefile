SRCS  = $(wildcard *.md)
HTMLS = $(SRCS:.md=.html)

all: html

html: ${HTMLS}

%.html: %.md
	pandoc -f gfm -s --toc --metadata title=$* $< -o $@

clean:
	rm -f *.html
