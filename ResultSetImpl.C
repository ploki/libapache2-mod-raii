#include "raii.H"

namespace raii {
namespace sql {


	bool ResultSetImpl::next() {

		int _rowCount=rowCount();
		if ( cursor >= _rowCount )
			throw SQLException("Cursor out of bound");
		++cursor;
		if ( cursor >= 0 && cursor < _rowCount )
			return true;
		return false;
	}

	bool ResultSetImpl::previous() {

		if ( cursor <= -1 )
			throw SQLException("Cursor out of bound");
		--cursor;
		if ( cursor >= 0 && cursor < int(rowCount()) )
			return true;
		return false;
	}

	void ResultSetImpl::first() {

		cursor = -1;
	}

	void ResultSetImpl::last() {

		cursor = rowCount();
	}

}
}

