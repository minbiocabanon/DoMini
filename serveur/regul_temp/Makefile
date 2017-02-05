PROG=regul_temp
$(PROG):$(PROG).c
	gcc -o $(PROG) $(PROG).c  `mysql_config --cflags --libs` -I"../include"
	cp $(PROG) ~/bin
	cp $(PROG) ../www/domini/bin 