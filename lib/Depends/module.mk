# the library
Depends_SRC =

Depends_INSTALL_HEADERS +=	DAG.h					\
				Depends.h				\
				Details/CircularReferenceException.h	\
				Details/Iterator.h			\
				Details/Node.h				\
				Details/ScopedFlag.h			\
				Details/SortPred.h			\
				Details/Unlinker.h			\
				Details/Visitors.h			\
				Details/serialization.hpp

SRC += $(patsubst %,lib/Depends/%,$(Depends_SRC))

INSTALL_HEADERS += $(patsubst %,Depends/%,$(Depends_INSTALL_HEADERS))
Depends_OBJ := $(patsubst %.cpp,lib/Depends/%.lo,$(Depends_SRC))
OBJ += $(Depends_OBJ)

$(eval $(call LINK_LIBRARY_template,libDepends.la,$(Depends_OBJ)))
