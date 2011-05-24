#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <app/documentmanager.h>

BOOST_AUTO_TEST_CASE(TestDocumentManager)
{
    DocumentManager manager;

    BOOST_CHECK_EQUAL(manager.getCurrentDocumentIndex(), -1);

    manager.createDocument();
    BOOST_CHECK_EQUAL(manager.getCurrentDocumentIndex(), 0);

    // should not be able to set document index to an invalid number
    manager.setCurrentDocumentIndex(1);
    BOOST_CHECK_EQUAL(manager.getCurrentDocumentIndex(), 0);

    // should be able to set document index to a valid number
    manager.createDocument();
    manager.createDocument();
    manager.setCurrentDocumentIndex(1);
    BOOST_CHECK_EQUAL(manager.getCurrentDocumentIndex(), 1);
}
