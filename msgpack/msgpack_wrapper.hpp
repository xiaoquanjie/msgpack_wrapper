/************************************************************************/
/* 也许有人觉得这里的设计是多余的，但是相信它能满足一部分人的需求。     */
/* 此库的设计原则是：“让你时刻知道你写入的是一种什么样格式的数据”		*/
/* author : xiaoquanjie													*/				
/************************************************************************/

#ifndef MSGPACK_WRAPPER_H
#define MSGPACK_WRAPPER_H

#include "msgpack.hpp"
#include <assert.h>
namespace MsgpackWrapper {

	class OObject;
	class OKVObject;

	class OMsgPackObject;
	class OMsgPackArrayObject;
	class OMsgPackMapObject;

	class IObject;
	class IKVObject;

	class IMsgPackObject;
	class IMsgPackArrayObject;
	class IMsgPackMapObject;
	
	typedef msgpack::sbuffer sbuffer; // 缓冲区

	//////////////////////////////////////////////////////////////////////////序列化相关数据结构////////////////////////////////////////////////////////

	class OObject
	{
	public:
		inline OObject(msgpack::sbuffer& s) :_s(s), _size(1) {
		}

		inline OObject(const OObject& o) : _s(o._s), _size(o._size) {
		}

		template<typename T>
		inline void Write(T const& v)
		{
			assert(--_size >= 0);
			msgpack::pack(_s, v);
		}

		template<int N>
		void Write(const char (&v)[N])
		{
			assert(--_size >= 0);
			msgpack::pack(_s, std::string(v));
		}

		OObject& operator=(const OObject& o)
		{
			_s = o._s;
			_size = o._size;
		}

	protected:
		int _size;
		msgpack::sbuffer& _s;
	};

	class OKVObject
	{
	public:
		inline OKVObject(msgpack::sbuffer& s) :_s(s), _size1(1), _size2(1) {
		}

		inline OKVObject(const OKVObject& o) : _s(o._s), _size1(o._size1), _size2(o._size2) {
		}

		inline OKVObject& operator=(const OKVObject& o)
		{
			_s = o._s;
			_size1 = o._size1;
			_size2 = o._size2;
			return *this;
		}

		OObject KeyObject()
		{
			assert(--_size1 >= 0);
			return OObject(_s);
		}

		OObject ValObject()
		{
			assert(--_size2 >= 0);
			return OObject(_s);
		}

	protected:
		int _size1;
		int _size2;
		msgpack::sbuffer& _s;
	};

	//////////////////////////////////////////////////////////////////////////序列化相关类////////////////////////////////////////////////////////

	// 单个值相关
	class OMsgPackObject
	{
	public:
		OMsgPackObject(msgpack::sbuffer& s) :_s(s) {}

		OObject Object()
		{
			return OObject(_s);
		}

		template<typename T>
		OMsgPackObject& WriteObject(T const& v)
		{
			Object().Write(v);
			return *this;
		}

		template<int N>
		OMsgPackObject& WriteObject(const char(&v)[N])
		{
			Object().Write(v);
			return *this;
		}

	protected:
		OMsgPackObject(const OMsgPackObject&);
		OMsgPackObject& operator=(const OMsgPackObject&);

	protected:
		msgpack::sbuffer& _s;
	};

	// 数组相关
	class OMsgPackArrayObject : public OMsgPackObject
	{
	public:
		OMsgPackArrayObject(msgpack::sbuffer& s, int n) :OMsgPackObject(s), _size(n)
		{
			msgpack::packer<msgpack::sbuffer>(_s).pack_array(n);
		}

		OObject Object()
		{
			assert(--_size >= 0);
			return OMsgPackObject::Object();
		}

		template<typename T>
		OMsgPackArrayObject& WriteObject(T const& v)
		{
			Object().Write(v);
			return *this;
		}

		template<int N>
		OMsgPackArrayObject& WriteObject(const char(&v)[N])
		{
			Object().Write(v);
			return *this;
		}

	protected:
		int _size;
	};

	// 键值对相关
	class OMsgPackMapObject : public OMsgPackObject
	{
	public:
		OMsgPackMapObject(msgpack::sbuffer& s, int n) :OMsgPackObject(s), _size(n)
		{
			msgpack::packer<msgpack::sbuffer>(_s).pack_map(n);
		}

		OKVObject Object()
		{
			assert(--_size >= 0);
			return OKVObject(_s);
		}

		template<typename T1,typename T2>
		OMsgPackMapObject& WriteObject(T1 const& k,T2 const& v)
		{
			OKVObject obj = Object();
			obj.KeyObject().Write(k);
			obj.ValObject().Write(v);
			return *this;
		}

		template<typename T,int N>
		OMsgPackMapObject& WriteObject(T const& k,const char(&v)[N])
		{
			OKVObject obj = Object();
			obj.KeyObject().Write(k);
			obj.ValObject().Write(v);
			return *this;
		}

		template<typename T, int N>
		OMsgPackMapObject& WriteObject(const char(&k)[N], T const& v)
		{
			OKVObject obj = Object();
			obj.KeyObject().Write(k);
			obj.ValObject().Write(v);
			return *this;
		}

		template<int N1, int N2>
		OMsgPackMapObject& WriteObject(const char(&k)[N1], const char(&v)[N2])
		{
			OKVObject obj = Object();
			obj.KeyObject().Write(k);
			obj.ValObject().Write(v);
			return *this;
		}

	protected:
		int _size;
	};

	//////////////////////////////////////////////////////////////////////////反序列化相关数据结构////////////////////////////////////////////////////////

	class IObject
	{
	public:
		IObject(msgpack::object* pobj) :_pobj(pobj) {}

		IObject(const IObject& o) :_pobj(o._pobj)
		{}

		IObject& operator=(const IObject& o)
		{
			_pobj = o._pobj;
			return *this;
		}

		template<typename T>
		void Read(T& v)
		{
			if (!_pobj)
				throw msgpack::type_error();

			_pobj->convert(&v);
		}

		msgpack::type::object_type Type()const
		{
			if (!_pobj)
				throw msgpack::type_error();

			return _pobj->type;
		}

		bool IsArray()const
		{
			return (Type() == msgpack::type::ARRAY);
		}

		bool IsMap()const
		{
			return (Type() == msgpack::type::MAP);
		}

	protected:
		msgpack::object* _pobj;
	};

	class IKVObject
	{
		typedef IKVObject MySelf;

	public:
		IKVObject(msgpack::object_kv* pobj) :_pobj(pobj) {}

		IKVObject(const MySelf& o) :_pobj(o._pobj)
		{}

		IKVObject& operator=(const IKVObject& o)
		{
			_pobj = o._pobj;
			return *this;
		}

		IObject KeyObject()
		{
			if (!_pobj)
				throw msgpack::type_error();

			return IObject(&_pobj->key);
		}

		IObject ValObject()
		{
			if (!_pobj)
				throw msgpack::type_error();

			return IObject(&_pobj->val);
		}

	protected:
		msgpack::object_kv* _pobj;
	};

	//////////////////////////////////////////////////////////////////////////反序列化相关类////////////////////////////////////////////////////////

	// 单个值相关
	class IMsgPackObject
	{
		typedef IMsgPackObject MySelf;

	public:
		IMsgPackObject(msgpack::unpacker& unpak)
		{
			if (!unpak.next(&_unpaked))
			{
				throw msgpack::type_error();
			}
		}

		IObject Object()
		{
			return IObject(&_unpaked.get());
		}

		template<typename T>
		IMsgPackObject& ReadObject(T& v)
		{
			Object().Read(v);
			return *this;
		}

	protected:
		IMsgPackObject(const IMsgPackObject&);
		IMsgPackObject& operator=(const IMsgPackObject&);

	protected:
		msgpack::unpacked _unpaked;
	};

	// 数组相关
	class IMsgPackArrayObject : public IMsgPackObject
	{
		typedef IMsgPackObject		Base;
		typedef IMsgPackArrayObject MySelf;

	public:
		IMsgPackArrayObject(msgpack::unpacker& unpak) : Base(unpak), _index(0)
		{
			if (_unpaked.get().type != msgpack::type::ARRAY)
				throw msgpack::type_error();
		}

		size_t Size()const
		{
			return _unpaked.get().via.array.size;
		}

		IObject Object()
		{
			if (_index >= _unpaked.get().via.array.size)
			{
				return IObject(0);
			}
			else
			{
				return IObject(_unpaked.get().via.array.ptr + _index++);
			}
		}

		template<typename T>
		IMsgPackArrayObject& ReadObject(T& v)
		{
			Object().Read(v);
			return *this;
		}

	protected:
		size_t _index;
	};

	// 键值对相关
	class IMsgPackMapObject : public IMsgPackObject
	{
		typedef IMsgPackObject		Base;
		typedef IMsgPackMapObject	MySelf;

	public:
		IMsgPackMapObject(msgpack::unpacker& unpak) : Base(unpak), _index(0)
		{
			if (_unpaked.get().type != msgpack::type::MAP)
				throw msgpack::type_error();
		}

		size_t Size()const
		{
			return _unpaked.get().via.map.size;
		}

		// 顺序迭代
		IKVObject Object()
		{
			if (_index >= _unpaked.get().via.map.size)
			{
				return IKVObject(0);
			}
			else
			{
				return IKVObject(_unpaked.get().via.map.ptr + _index++);
			}
		}

		// key值索引,速度会慢,但是安全
		template<typename T>
		IKVObject Object(const T& key)
		{
			for (size_t idx=0; idx<_unpaked.get().via.map.size; ++idx)
			{
				msgpack::object_kv* pkv = _unpaked.get().via.map.ptr + idx;
				try { 
					if (pkv->key.as<T>() == key)
						return IKVObject(_unpaked.get().via.map.ptr + idx++);
				}catch(...){}
			}
			return IKVObject(0);
		}

		// key值索引,速度会有问题,但是安全
		template<int N>
		IKVObject Object(const char(&k)[N])
		{
			return Object(std::string(k));
		}

		// key值是非加const的
		template<typename T1, typename T2>
		IMsgPackMapObject& ReadObject(T1& k,T2& v)
		{
			IKVObject obj = Object();
			obj.KeyObject().Read(k);
			obj.ValObject().Read(v);
			return *this;
		}

		// key值是加const的
		template<typename T1, typename T2>
		IMsgPackMapObject& ReadObject(const T1& k, T2& v)
		{
			IKVObject obj = Object(k);
			obj.ValObject().Read(v);
			return *this;
		}

		template<typename T, int N>
		IMsgPackMapObject& ReadObject(const char(&k)[N],T& v)
		{
			IKVObject obj = Object(k);
			obj.ValObject().Read(v);
			return *this;
		}

	protected:
		size_t _index;
	};

	// 
	class MsgUnpacker
	{
	public:
		MsgUnpacker(msgpack::sbuffer& buffer)
		{
			_unpacker.reserve_buffer(buffer.size());
			memcpy(_unpacker.buffer(), buffer.data(), buffer.size());
			_unpacker.buffer_consumed(buffer.size());
		}

		msgpack::unpacker& unpacker()
		{
			return _unpacker;
		}

		operator msgpack::unpacker&()
		{
			return _unpacker;
		}

	private:
		msgpack::unpacker _unpacker;
	};
}

#define mpw MsgpackWrapper
#endif