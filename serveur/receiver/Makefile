PROG=receiver
$(PROG):$(PROG).c
	gcc -I"../include" -lmysqlclient -lz -DWITHMYSQL -o $(PROG) $(PROG).c
	cp $(PROG) ~/bin
