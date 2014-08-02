PROG=del_old_data
$(PROG):$(PROG).c passwd.h
	gcc -lmysqlclient -lz -DWITHMYSQL -D_DEBUG_ -o $(PROG) $(PROG).c
	cp $(PROG) /root/bin
