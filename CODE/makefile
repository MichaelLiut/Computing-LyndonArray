testbed_idla: testbed.cpp bsla.cpp Lstring.hpp debug.hpp lynarr.hpp
	g++ -o testbed_idla testbed.cpp -D_IDLA
	mv testbed_idla ..
silent_testbed_idla: testbed.cpp bsla.cpp Lstring.hpp debug.hpp lynarr.hpp
	g++ -o silent_testbed_idla testbed.cpp -D_IDLA -D_silent
	mv silent_testbed_idla ..
testbed_trla: testbed.cpp bsla.cpp Lstring.hpp debug.hpp lynarr.hpp
	g++ -o testbed_trla testbed.cpp -D_TRLA
	mv testbed_trla ..
silent_testbed_trla: testbed.cpp bsla.cpp Lstring.hpp debug.hpp lynarr.hpp
	g++ -o silent_testbed_trla testbed.cpp -D_TRLA -D_silent
	mv silent_testbed_trla ..
testbed_bsla: testbed.cpp bsla.cpp Lstring.hpp debug.hpp lynarr.hpp
	g++ -o testbed_bsla testbed.cpp -D_BSLA
	mv testbed_bsla ..
silent_testbed_bsla: testbed.cpp bsla.cpp Lstring.hpp debug.hpp lynarr.hpp
	g++ -o silent_testbed_bsla testbed.cpp -D_BSLA -D_silent
	mv silent_testbed_bsla ..
bsla: bsla.cpp Lstring.hpp lynarr.hpp
	g++ -o bsla bsla.cpp -D_stand_alone
	mv bsla ..
trla: trla.cpp Lstring.hpp lynarr.hpp debug.hpp
	g++ -o trla trla.cpp -D_stand_alone
	mv trla ..
