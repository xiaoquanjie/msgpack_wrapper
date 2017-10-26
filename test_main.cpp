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
		OMsgPackObject(aoBuffer).WriteObject("xiaoquanjie").WriteObject("nihaoma");

		// 数组
		OMsgPackArrayObject array_object(aoBuffer, 3);
		array_object.Object().Write("wo");
		array_object.WriteObject("ai").WriteObject("ni");

		// kv值
		OMsgPackMapObject map_object(aoBuffer, 2);
		OKVObject okv = map_object.Object();
		okv.KeyObject().Write("1");
		okv.ValObject().Write("woaini");
		map_object.WriteObject(1, "woaini2");
	}

	void Read(msgpack::sbuffer& aoBuffer)
	{
		MsgUnpacker aoUnpak(aoBuffer);
		cout << "~~~~~~~~~~~~~~~" << endl;

		// 单个值
		std::string value;
		IMsgPackObject(aoUnpak).ReadObject(value);
		cout << value << endl;
		IMsgPackObject(aoUnpak).Object().Read(value);
		cout << value << endl;
		
		// 数组
		IMsgPackArrayObject array_object(aoUnpak);
		for (int i = 0; i < array_object.Size(); ++i)
		{
			//array_object.Object().Read(value);
			array_object.ReadObject(value);
			cout << value << endl;
		}

		// kv值
		IMsgPackMapObject map_object(aoUnpak);
		/*for (int i = 0; i < map_object.Size(); ++i)
		{
			IKVObject object = map_object.Object();
			std::string k;
			std::string v;
			object.ObjectKey().Read(k);
			object.ObjectVal().Read(v);
			cout << k << " " << v << endl;
		}*/

		// kv值
		{
			const int k = 1;
			std::string v;
			map_object.ReadObject("1", v);

			cout << k << " " << v << endl;
		}
	}
};

#define THIS_FILE (_tcsrchr(_TEXT(__FILE__), _TEXT('\\')) ? _tcsrchr(_TEXT(__FILE__), _TEXT('\\')) : _TEXT(__FILE__))

typedef std::vector<int> int_vec_type;
typedef std::vector<int_vec_type>  vec_vec_type;
typedef std::map<int, vec_vec_type> int_vecvec_map;
void _GetCombination(const std::vector<int>& src_vec, int cnt, int_vecvec_map& tar_map)
{
	int total_cnt = src_vec.size();
	int bit_cnt = 1 << total_cnt;
	for (int i = 0; i <= bit_cnt; ++i)
	{
		std::vector<int> vec;
		for (int j = 0; j < total_cnt; ++j)
		{
			if ((i << (31 - j)) >> 31 == -1)
				vec.push_back(src_vec[j]);
		}
		// 隔开
		if (vec.size())
			tar_map[vec.size()].push_back(vec);
	}
}

int main(void)
{
	Stru_Test st;
	mpw::sbuffer loBuffer;
	st.Write(loBuffer);
	st.Print(loBuffer);
	st.Read(loBuffer);


	return 0;
}
