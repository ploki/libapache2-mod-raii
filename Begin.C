#include "raii.H"

namespace raii {
	namespace sql {

		void Begin::setIsolationLevel() {

			if (noop) return;

			switch (isolation) {
				case SERIALIZABLE:
					conn.query("SET TRANSACTION ISOLATION LEVEL SERIALIZABLE");
					break;
				case REPEATABLE_READ:
					conn.query("SET TRANSACTION ISOLATION LEVEL REPEATABLE READ");
					break;
				case READ_COMMITTED:
					conn.query("SET TRANSACTION ISOLATION LEVEL READ COMMITTED");
					break;
				case READ_UNCOMMITTED:
					conn.query("SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED");
					break;
				case READ_WRITE:
					conn.query("SET TRANSACTION ISOLATION LEVEL READ WRITE");
					break;
				case READ_ONLY:
					conn.query("SET TRANSACTION ISOLATION LEVEL READ_ONLY");
					break;
				default:
					throw SQLException("Niveau d'isolation inconnu");
			}
		}


		void Begin::resetIsolationLevel() { setIsolationLevelReadCommitted(); }
		void Begin::setIsolationLevelSerializable() { isolation=SERIALIZABLE; setIsolationLevel(); }
		void Begin::setIsolationLevelRepeatableRead() { isolation=REPEATABLE_READ; setIsolationLevel(); }
		void Begin::setIsolationLevelReadCommitted() { isolation=READ_COMMITTED; setIsolationLevel(); }
		void Begin::setIsolationLevelReadUncommitted() { isolation=READ_UNCOMMITTED; setIsolationLevel(); }
		void Begin::setIsolationLevelReadWrite() { isolation=READ_WRITE; setIsolationLevel(); }
		void Begin::setIsolationLevelReadOnly() { isolation=READ_ONLY; setIsolationLevel(); }

		Begin::Begin(Connection& c, bool f) : conn(c), isolation(READ_COMMITTED), noop(false), factice(f), used(false) {
			if (factice) return;

			if ( conn.isAutoCommitOn() )
				conn.autoCommitOff();
			else
				noop=true;
		}

		void Begin::commit() {
			used=true;
			if (factice || noop) return;
			conn.commit();
			setIsolationLevel();
		}

		void Begin::rollback() {
			used=true;
			if (factice)
				throw SQLException("ROLLBACK sur une transaction factice, les requêtes en cours peuvent avoir été inscrites en base");
			if (noop)
				throw SQLException("ROLLBACK depuis une sous-transaction qui n'a pas connaissance de la transaction parente");
			conn.rollback();
			setIsolationLevel();
		}

		Begin::~Begin() {
			if ( !used ) {
				//affichage de la pile et d'un message
				Throwable e;
				fprintf(stderr,"*** Begin transaction never used and RAII pattern launched a rollback ***\n");
				fprintf(stderr,"%s\n",e.getMessage().c_str());
				fflush(stderr);
				e.printStackTrace();
			}
			if (factice || noop) return;
			conn.autoCommitOn();
		}

}
}
