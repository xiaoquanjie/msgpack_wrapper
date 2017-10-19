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
	class OArrayObject;
	class OMapObject;
	class OKVObject;

	class OMsgPackObject;
	class OMsgPackArrayObject;
	class OMsgPackMapObject;

	class IObject;
	class IArrayObject;
	class IMapObject;
	class IKVObject;

	class IMsgPackObject;
	class IMsgPackArrayObject;
	class IMsgPackMapObject;

	typedef msgpack::sbuffer sbuffer; // 缓冲区

	//////////////////////////////////////////////////////////////////////////序列化相关数据结构////////////////////////////////////////////////////////

	class OObject
	{
		friend OArrayObject;
		friend OMapObject;

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

	class OArrayObject
	{
	public:
		OArrayObject(const OObject& o, int n) :_s(o._s), _size(n)
		{
			msgpack::packer<msgpack::sbuffer>(_s).pack_array(n);
		}

		OArrayObject(const OArrayObject& o) : _s(o._s), _size(o._size) {
		}

		inline OArrayObject& operator=(const OArrayObject& o)
		{
			_size = o._size;
			_s = o._s;
			return *this;
		}

		OObject Object()
		{
			assert(--_size >= 0);
			return OObject(_s);
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

		OObject ObjectKey()
		{
			assert(--_size1 >= 0);
			return OObject(_s);
		}

		OObject ObjectVal()
		{
			assert(--_size2 >= 0);
			return OObject(_s);
		}

	protected:
		int _size1;
		int _size2;
		msgpack::sbuffer& _s;
	};

	class OMapObject
	{
	public:
		OMapObject(const OObject& o, size_t n) :_s(o._s), _size(n)
		{
			msgpack::packer<msgpack::sbuffer>(_s).pack_map(n);
		}

		OMapObject(const OMapObject& o) : _s(o._s), _size(o._size) {
		}

		OKVObject Object()
		{
			assert(--_size >= 0);
			return OKVObject(_s);
		}

		inline OMapObject& operator=(const OMapObject& o)
		{
			_size = o._size;
			_s = o._s;
			return *this;
		}

	protected:
		int _size;
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

	protected:
		int _size;
	};

	//////////////////////////////////////////////////////////////////////////反序列化相关数据结构////////////////////////////////////////////////////////

	class IObject
	{
		friend IArrayObject;
		friend IMapObject;

		typedef IObject MySelf;

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

	class IArrayObject
	{
		typedef IObject			Base;
		typedef IArrayObject	MySelf;

	public:
		IArrayObject(const IObject& b) : _pobj(b._pobj), _index(0) {}

		IArrayObject(const MySelf& o) :_pobj(o._pobj), _index(o._index) {}

		IArrayObject& operator=(const IArrayObject& o)
		{
			_pobj = o._pobj;
			_index = o._index;
			return *this;
		}

		IObject Object()
		{
			if (!_pobj)
				throw msgpack::type_error();

			if (_index >= _pobj->via.array.size)
			{
				return IObject(0);
			}
			else
			{
				return IObject(_pobj->via.array.ptr + _index++);
			}
		}

	protected:
		size_t _index;
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

		IObject ObjectKey()
		{
			if (!_pobj)
				throw msgpack::type_error();

			return IObject(&_pobj->key);
		}

		IObject ObjectVal()
		{
			if (!_pobj)
				throw msgpack::type_error();

			return IObject(&_pobj->val);
		}

	protected:
		msgpack::object_kv* _pobj;
	};

	class IMapObject
	{
		typedef IObject		Base;
		typedef IMapObject	MySelf;

	public:
		IMapObject(const IObject& b) : _pobj(b._pobj), _index(0) {}

		IMapObject(const IMapObject& o) : _pobj(o._pobj), _index(o._index) {}

		IMapObject& operator=(const IMapObject& o)
		{
			_pobj = o._pobj;
			_index = o._index;
			return *this;
		}

		IKVObject Object()
		{
			if (!_pobj)
				throw msgpack::type_error();

			if (_index >= _pobj->via.map.size)
			{
				return IKVObject(0);
			}
			else
			{
				return IKVObject(_pobj->via.map.ptr + _index++);
			}
		}

	protected:
		size_t _index;
		msgpack::object* _pobj;
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

	protected:
		size_t _index;
	};

}

#define mpw MsgpackWrapper
#endif