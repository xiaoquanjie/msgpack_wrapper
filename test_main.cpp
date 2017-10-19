#include <iostream>
#include "msgpack_wrapper.hpp"
#include <memory>

using namespace std;
using namespace MsgpackWrapper;

#define NAME_TO_STR(var)  (#var)
#define WriteKeyValue(loPack, key,value ) loPack.pack( key ); loPack.pack( value );


struct Stru_Test
{
	void Print(msgpack::sbuffer& aoBuffer)
	{
		msgpack::unpacker aoUnpak;
		aoUnpak.reserve_buffer(aoBuffer.size());
		memcpy(aoUnpak.buffer(), aoBuffer.data(), aoBuffer.size());
		aoUnpak.buffer_consumed(aoBuffer.size());

		msgpack::unpacked result_;
		while (aoUnpak.next(&result_))
		{
			std::cout << result_.get() << std::endl;
		}
	}

	void Write(msgpack::sbuffer& aoBuffer)
	{
		OMsgPackObject opObject(aoBuffer);
		OObject tmp = opObject.Object();// .Write(2);
		tmp.Write(2);

		OMsgPackArrayObject array_object(aoBuffer, 3);
		array_object.Object().Write(1);
		array_object.Object().Write(2);

		OArrayObject arrobject(array_object.Object(), 3);
		arrobject.Object().Write(44);
		arrobject.Object().Write(55);
		//arrobject.Object().Write(66);
		
		OMapObject mapobject(arrobject.Object(), 1);
		OKVObject okvobject = mapobject.Object();
		okvobject.ObjectKey().Write(100);
		okvobject.ObjectVal().Write(110);
		
		OMsgPackMapObject pakmapobject(aoBuffer, 2);
		okvobject = pakmapobject.Object();
		okvobject.ObjectKey().Write(std::string("xiao"));
		okvobject.ObjectVal().Write(std::string("xiao"));
		okvobject = pakmapobject.Object();
		okvobject.ObjectKey().Write(std::string("xiao"));
		
		OMapObject omapobject(okvobject.ObjectVal(),1);
		okvobject = omapobject.Object();
		okvobject.ObjectKey().Write(200);
		//okvobject.ObjectKey().Write(300);
		okvobject.ObjectVal().Write(200);
	}

	void Read(msgpack::sbuffer& aoBuffer)
	{
		return ;

		msgpack::unpacker aoUnpak;
		aoUnpak.reserve_buffer(aoBuffer.size());
		memcpy(aoUnpak.buffer(), aoBuffer.data(), aoBuffer.size());
		aoUnpak.buffer_consumed(aoBuffer.size());

		cout << "~~~~~~~~~~~~~~~" << endl;
		IMsgPackObject iObject(aoUnpak);
		int i = 0;
		iObject.Object().Read(i);
		cout << i << endl;

		IMsgPackArrayObject array_object(aoUnpak);
		int j = 0;
		array_object.Object().Read(j);
		cout << j << endl;
		array_object.Object().Read(j);
		cout << j << endl;

		IObject object = array_object.Object();
		IArrayObject aobject(object);
		aobject.Object().Read(j);
		cout << j << endl;
		aobject.Object().Read(j);
		cout << j << endl;
		aobject.Object().Read(j);
		cout << j << endl;

		IMapObject mapobject(array_object.Object());
		mapobject.Object().ObjectKey().Read(j);
	}
};

#define THIS_FILE (_tcsrchr(_TEXT(__FILE__), _TEXT('\\')) ? _tcsrchr(_TEXT(__FILE__), _TEXT('\\')) : _TEXT(__FILE__))

int main(void)
{
	Stru_Test st;
	mpw::sbuffer loBuffer;
	st.Write(loBuffer);
	st.Print(loBuffer);
	st.Read(loBuffer);


	return 0;
}
