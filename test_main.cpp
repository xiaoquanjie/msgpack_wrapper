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
		// 单个value值
		OMsgPackObject(aoBuffer).Object().Write("xiaoquanjie");
		OMsgPackObject(aoBuffer).Object().Write("nihaoma");

		// 数组
		OMsgPackArrayObject array_object(aoBuffer, 3);
		array_object.Object().Write("wo");
		array_object.Object().Write("ai");
		array_object.Object().Write("ni");

		// kv值
		OMsgPackMapObject map_object(aoBuffer, 1);
		OKVObject okv = map_object.Object();
		okv.ObjectKey().Write(1);
		okv.ObjectVal().Write("woaini");

	}

	void Read(msgpack::sbuffer& aoBuffer)
	{
		msgpack::unpacker aoUnpak;
		aoUnpak.reserve_buffer(aoBuffer.size());
		memcpy(aoUnpak.buffer(), aoBuffer.data(), aoBuffer.size());
		aoUnpak.buffer_consumed(aoBuffer.size());

		cout << "~~~~~~~~~~~~~~~" << endl;

		// 单个值
		std::string value;
		IMsgPackObject(aoUnpak).Object().Read(value);
		cout << value << endl;
		IMsgPackObject(aoUnpak).Object().Read(value);
		cout << value << endl;
		
		// 数组
		IMsgPackArrayObject array_object(aoUnpak);
		for (int i = 0; i < array_object.Size(); ++i)
		{
			array_object.Object().Read(value);
			cout << value << endl;
		}

		// kv值
		IMsgPackMapObject map_object(aoUnpak);
		for (int i = 0; i < map_object.Size(); ++i)
		{
			IKVObject object = map_object.Object();
			int k;
			std::string v;
			object.ObjectKey().Read(k);
			object.ObjectVal().Read(v);
			cout << k << " " << v << endl;
		}
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
