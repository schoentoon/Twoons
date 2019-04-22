SHELL       := /bin/bash
LDFLAGS     := -I. -Ijson -Iui -Itwitter -Idatastorage -Ipicture -Iuploading -Inotif
CXX         := g++
CC          := gcc
LINKER      := -Wl,--export-dynamic
VERSION     := $(shell git describe)
DEFINES     := -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DVERSION=\"${VERSION}\"
PKGCONFIG   := pkg-config
ARCH        := -m64
CFLAGS      := -pipe -O2 -Wall -mtune=generic -Wp,-D_FORTIFY_SOURCE=2 -fstack-protector --param=ssp-buffer-size=4 ${ARCH} -W -D_REENTRANT ${DEFINES} ${EXTRA}
ifdef DEBUG
	CFLAGS := $(CFLAGS) -g
endif
INCPATH     := $(shell ${PKGCONFIG} --libs QtCore QtGui QtNetwork QtDBus)
LFLAGS      := -Wl,-O1
LIBS        := $(shell ${PKGCONFIG} --cflags QtCore QtGui QtNetwork QtDBus)
EXECUTABLE  := twoons
MOCBIN      := /usr/lib64/qt4/bin/moc
MOCS        := moc_datastorage.datastorage moc_twitter.stream_thread moc_ui.ui_settings moc_ui.ui_errorbox moc_picture.framework \
moc_ui.ui_auth moc_ui.ui_autocomplete moc_ui.ui_column moc_ui.ui_filtersettings moc_ui.ui_trendingtopic moc_ui.ui_imageview \
moc_ui.ui_listbox moc_ui.ui_mainwindow moc_ui.ui_tweetbox moc_ui.ui_tweetfield moc_ui.ui_userbox moc_ui.ui_userprofile moc_ui.ui_followbutton \
moc_picture.picdotwitter moc_picture.picplz moc_picture.flickr moc_uploading.upframework moc_ui.ui_conversationview moc_ui.ui_dm moc_ui.ui_dmuser moc_ui.ui_dmview \
moc_notif.orgfreedesktopnotificationsinterface moc_notif.notification 
LIB_SRCS    := CString SHA256 MD5 OAuthUtils OAuth debug json/jsonxx ui/ui_errorbox ui/ui_conversationview \
datastorage/datastorage datastorage/user_store datastorage/global ui/ui_settings ui/ui_trendingtopic \
ui/ui_column ui/ui_mainwindow ui/ui_tweetbox ui/ui_filtersettings ui/ui_userprofile ui/ui_imageview ui/ui_followbutton \
ui/ui_listbox ui/ui_tweetfield ui/ui_userbox ui/ui_autocomplete ui/ui_auth twitter/stream_thread \
twitter/status twitter/user twitter/list uploading/upframework uploading/twitpic uploading/imgur uploading/imgurd uploading/imgly \
picture/framework picture/picplz picture/twitpic picture/yfrog picture/imgur picture/instagram picture/picdotwitter picture/lockerz picture/imgly picture/flickr \
picture/mobyto ui/ui_dm ui/ui_dmuser ui/ui_dmview notif/orgfreedesktopnotificationsinterface notif/notification main

all: mocs $(LIB_SRCS) resources link

%: %.cpp
	$(CXX) $(CFLAGS) $(INCPATH) $(LIBS) $(LDFLAGS) -c -o $@.o $@.cpp

moc_%:
	$(MOCBIN) -i $(subst moc_,,$(subst .,/,$@)).h -o $(subst .,/moc_,$(subst moc_,,$@)).h
	@sed -i '1,16d' $(subst .,/moc_,$(subst moc_,,$@)).h

mocs: $(MOCS)

resources:
	rcc twoons.qrc -o resources.cpp
	@sed -i '1,8d' resources.cpp
	@sed -i '/\/\//d' resources.cpp
	$(CC) $(CFLAGS) $(INCPATH) $(LIBS) $(LDFLAGS) -c resources.cpp -o resources.o

link: $(LIB_SRCS) resources
	$(CXX) $(CFLAGS) $(INCPATH) $(LIBS) $(LFLAGS) -o $(EXECUTABLE) *.o json/*.o ui/*.o twitter/*.o datastorage/*.o picture/*.o uploading/*.o notif/*.o

quick_link:
	$(CXX) $(CFLAGS) $(INCPATH) $(LIBS) $(LFLAGS) -o $(EXECUTABLE) *.o json/*.o ui/*.o twitter/*.o datastorage/*.o picture/*.o uploading/*.o notif/*.o

install:
	cp $(EXECUTABLE) ~/bin/

clean:
	rm -f *.o json/*.o ui/*.o twitter/*.o datastorage/*.o picture/*.o *~ $(EXECUTABLE)

dist: $(EXECUTABLE)
	md5sum $(EXECUTABLE)
	sha1sum $(EXECUTABLE)
	sha512sum $(EXECUTABLE)
	@gzip -v -f $(EXECUTABLE)
	@echo "Now go upload $(EXECUTABLE).gz"

doc:
	doxygen

tags:
	ctags -R --exclude=html/ --exclude=*.o .
