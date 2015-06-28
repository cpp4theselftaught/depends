Test_DAG_SRC :=			test.cc
Test_Depends_SRC :=		test2.cc
Test_serializeDAG_SRC :=	test3.cc
Test_serializeDepends_SRC :=	test4.cc
SRC += $(patsubst %,tests/lib/Depends/%,$(Test_DAG_SRC))
SRC += $(patsubst %,tests/lib/Depends/%,$(Test_Depends_SRC))
SRC += $(patsubst %,tests/lib/Depends/%,$(Test_serializeDAG_SRC))
SRC += $(patsubst %,tests/lib/Depends/%,$(Test_serializeDepends_SRC))

Test_DAG_OBJ := $(patsubst %.cc,tests/lib/Depends/%.lo,$(Test_DAG_SRC))
Test_Depends_OBJ := $(patsubst %.cc,tests/lib/Depends/%.lo,$(Test_Depends_SRC))
Test_serializeDAG_OBJ := $(patsubst %.cc,tests/lib/Depends/%.lo,$(Test_serializeDAG_SRC))
Test_serializeDepends_OBJ := $(patsubst %.cc,tests/lib/Depends/%.lo,$(Test_serializeDepends_SRC))

OBJ += $(Test_DAG_OBJ) $(Test_Depends_OBJ) $(Test_serializeDAG_OBJ) $(Test_serializeDepends_OBJ)

TEST_LDFLAGS=libDepends.la $(LIBS)
$(eval $(call LINK_BINARY_template,Test_DAG.bin,$(Test_DAG_OBJ),$(TEST_LDFLAGS)))
$(eval $(call LINK_BINARY_template,Test_Depends.bin,$(Test_Depends_OBJ),$(TEST_LDFLAGS)))
CHECK_DEPS += Test_DAG.bin Test_Depends.bin 

ifeq ($(ENABLE_SERIALIZATION),1)
TEST_LDFLAGS+=-lboost_serialization
endif
$(eval $(call LINK_BINARY_template,Test_serializeDAG.bin,$(Test_serializeDAG_OBJ),$(TEST_LDFLAGS)))
$(eval $(call LINK_BINARY_template,Test_serializeDepends.bin,$(Test_serializeDepends_OBJ),$(TEST_LDFLAGS)))
CHECK_DEPS += Test_serializeDAG.bin Test_serializeDepends.bin

