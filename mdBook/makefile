
SRCS := $(shell find ../Book -type f)
THEME := $(shell find ./theme -type f)

mdBook: $(SRCS) $(THEME) ./book.toml ./src/SUMMARY.md
	rsync -av --delete ../Book/ ./src/Book/
	mdbook build
	touch mdBook


run: mdBook
	mdbook serve


clean:
	rm -rf ./book
	rm -rf ./src/Book
	rm -f ./mdBook

