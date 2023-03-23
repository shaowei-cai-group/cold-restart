all:
	$(MAKE) -C "/home/chenzh/solvers/shuffle/pakis-mab-sr/kissat_mab/build"
kissat:
	$(MAKE) -C "/home/chenzh/solvers/shuffle/pakis-mab-sr/kissat_mab/build" kissat
tissat:
	$(MAKE) -C "/home/chenzh/solvers/shuffle/pakis-mab-sr/kissat_mab/build" tissat
clean:
	rm -f "/home/chenzh/solvers/shuffle/pakis-mab-sr/kissat_mab"/makefile
	-$(MAKE) -C "/home/chenzh/solvers/shuffle/pakis-mab-sr/kissat_mab/build" clean
	rm -rf "/home/chenzh/solvers/shuffle/pakis-mab-sr/kissat_mab/build"
coverage:
	$(MAKE) -C "/home/chenzh/solvers/shuffle/pakis-mab-sr/kissat_mab/build" coverage
indent:
	$(MAKE) -C "/home/chenzh/solvers/shuffle/pakis-mab-sr/kissat_mab/build" indent
test:
	$(MAKE) -C "/home/chenzh/solvers/shuffle/pakis-mab-sr/kissat_mab/build" test
.PHONY: all clean coverage indent kissat test tissat
