PROG=receiver
$(PROG):$(PROG).c passwd.h
	gcc -lmysqlclient -lz -DWITHMYSQL -o $(PROG) $(PROG).c
	cp $(PROG) /root/bin
