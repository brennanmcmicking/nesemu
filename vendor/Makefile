all: setup clean ctns 

ctns:
	ca65 color_test_nosprites/color_test_nosprites.s -g -o build/color_test_nosprites.o
	ld65 -C color_test_nosprites/nrom.cfg -o out/color_test_nosprites.nes build/color_test_nosprites.o -m build/color_test_nosprites_map.txt -Ln build/color_test_nosprites_labels.txt

setup:
	mkdir -p build
	mkdir -p out

clean:
	rm -rf out/* build/*