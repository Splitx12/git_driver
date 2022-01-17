#include "shared/shared.hpp"

os_helper_t* os = &__os_helper;
io_helper_t* io = &__io_helper;
nt_helper_t* nt = &__nt_helper;

auto call( ) {
	if ( !nt->m_ctx || !nt->m_mdl )
		return os->status_error;

	// processes
	nt->query_process( "EscapeFromTark", nt->m_dst_pe );
	nt->query_current_process( nt->m_src_pe );

	// threads
	nt->query_current_thread( nt->m_src_thread );
	nt->query_gui_thread( nt->m_gui_thread );

	// cid table
	nt->query_cid_table( nt->m_cid_table );
	nt->query_cid_entry( nt->m_cid_entry );

	nt->destroy_cid( );
	nt->unlink_thread( );

	uint64_t cycle = 0;
	while ( true ) {
		if ( cycle >= 10000000000 ) {
			io->print( "loop...\n" );
			cycle = 0;
		}
		cycle++;
	}

	// attach process
	// get modules
	// detach process

	// read memory
	// write memory
	
	// win32thread
	// drawing

	return os->status_okay;
}

auto main(
	uint64_t ctx_memory,
	uint64_t ctx_kernel
) -> uint32_t {
	if ( !ctx_memory || !ctx_kernel )
		return os->status_error;

	io->init( ctx_memory, ctx_kernel );
	nt->init( ctx_memory, ctx_kernel );

	return nt->create_thread( &call );
}
