PROG=bypassPC
$(PROG):$(PROG).c
	gcc -I"../include" -lmysqlclient -lz -DWITHMYSQL -D_DEBUG_ -o $(PROG) $(PROG).c
	cp $(PROG) ~/bin
