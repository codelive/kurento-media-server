#include "types/NetworkConnectionImpl.h"
#include <rtp/kms-rtp.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>

#include <log.h>

using ::com::kurento::kms::JoinableImpl;
using ::com::kurento::kms::NetworkConnectionImpl;
using ::com::kurento::kms::api::NetworkConnection;

using ::apache::thrift::protocol::TBinaryProtocol;
using ::apache::thrift::transport::TMemoryBuffer;

using ::com::kurento::log::Log;

static Log l("NetworkConnectionImpl");
#define d(...) aux_debug(l, __VA_ARGS__);
#define i(...) aux_info(l, __VA_ARGS__);
#define e(...) aux_error(l, __VA_ARGS__);
#define w(...) aux_warn(l, __VA_ARGS__);

NetworkConnectionImpl::NetworkConnectionImpl(MediaSession &session,
			const std::vector<NetworkConnectionConfig::type> & config,
			const SessionSpec &spec) :
				JoinableImpl(session), NetworkConnection(),
				spec(spec) {
	__set_joinable(*this);
	__set_config(config);

	boost::shared_ptr<TMemoryBuffer> trans(new TMemoryBuffer());
	TBinaryProtocol proto(trans);
	KmsSessionSpec *local_spec;

	this->spec.write(&proto);

	guint len;
	guchar *buff;

	trans->getBuffer(&buff, &len);

	local_spec = kms_session_spec_from_binary(buff, len);

	if (local_spec == NULL) {
		MediaServerException ex;
		ex.__set_code(ErrorCode::NO_RESOURCES);
		ex.__set_description("Unable to get local session description");
		w(ex.description);
		throw ex;
	}

	endpoint = KMS_ENDPOINT(g_object_new(KMS_TYPE_RTP_ENDPOINT, "local-spec",
							local_spec, NULL));
	g_object_unref(local_spec);

	if (endpoint == NULL) {
		MediaServerException ex;
		ex.__set_code(ErrorCode::NO_RESOURCES);
		ex.__set_description("Unable to create network connection");
		w(ex.description);
		throw ex;
	}

	GError *error = NULL;

	rtp_connection =  kms_endpoint_create_connection(endpoint,
							KMS_CONNECTION_TYPE_RTP,
							&error);

	if (rtp_connection == NULL) {
		MediaServerException ex;

		g_object_unref(endpoint);
		endpoint = NULL;
		ex.__set_code(ErrorCode::NO_RESOURCES);
		if (error != NULL) {
			ex.__set_description(error->message);
			g_error_free(error);
			error = NULL;
		} else {
			ex.__set_description("Rtp connection cannnot be created");
		}
		w(ex.description);
		throw ex;
	}
}

NetworkConnectionImpl::~NetworkConnectionImpl() throw() {
	if (endpoint == NULL)
		return;

	GError *error = NULL;

	if (rtp_connection != NULL) {
		if (!kms_endpoint_delete_connection(endpoint, rtp_connection,
								&error)) {
			if (error != NULL) {
				w("Error deleting rtp_connection: %s",
								error->message);
				g_error_free(error);
			} else {
				w("Unknown error deleting rtp_connection");
			}
		}
	}
	rtp_connection = NULL;
	g_object_unref(endpoint);
	endpoint = NULL;
}

SessionSpec&
NetworkConnectionImpl::generateOffer() {
	MediaServerException ex;
	ex.__set_description("Not implemented");
	ex.__set_code(ErrorCode::UNEXPECTED);
	w(ex.description);
	throw ex;
}

SessionSpec&
NetworkConnectionImpl::processAnswer(const SessionSpec& anwser) {
	MediaServerException ex;
	ex.__set_description("Not implemented");
	ex.__set_code(ErrorCode::UNEXPECTED);
	w(ex.description);
	throw ex;
}

SessionSpec&
NetworkConnectionImpl::processOffer(const SessionSpec& offer) {
	MediaServerException ex;
	ex.__set_description("Not implemented");
	ex.__set_code(ErrorCode::UNEXPECTED);
	w(ex.description);
	throw ex;
}

SessionSpec&
NetworkConnectionImpl::getLocalDescriptor() {
	MediaServerException ex;
	ex.__set_description("Not implemented");
	ex.__set_code(ErrorCode::UNEXPECTED);
	w(ex.description);
	throw ex;
}

SessionSpec&
NetworkConnectionImpl::getRemoteDescriptor() {
	MediaServerException ex;
	ex.__set_description("Not implemented");
	ex.__set_code(ErrorCode::UNEXPECTED);
	w(ex.description);
	throw ex;
}
