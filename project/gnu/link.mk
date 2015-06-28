Tests.bin : $(Tests_OBJ)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $^ $(LIBS)
