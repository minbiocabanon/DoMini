PROG=regul_temp
$(PROG):$(PROG).c passwd.h
	gcc -lmysqlclient -lz -DWITHMYSQL -D_DEBUG_ -o $(PROG) $(PROG).c
	cp $(PROG) ~/bin
	cp $(PROG) /var/www/domini/bin 
