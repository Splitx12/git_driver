#pragma once

struct kernel_t {
   [[ nodiscard ]]
   const std::ptrdiff_t get_cid_table( ) {
      auto ctx{ ptr< std::uint8_t* >( m_ntos ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x1e ] != 0x69
           || ctx[ 0x1f ] != 0x48
           || ctx[ 0x20 ] != 0x8b
           || ctx[ 0x21 ] != 0x0d
           || ctx[ 0x26 ] != 0x4c )
         ctx++;

      auto cast{ *ptr< std::int32_t* >( &ctx[ 0x0 ] ) };
      if ( !cast )
         return 0;

      return *ptr< std::ptrdiff_t* >( &ctx[ 0x4 ] + cast );
   }

   [[ nodiscard ]]
   const std::ptrdiff_t get_initial_process( ) {
      auto ctx{ ptr< std::uint8_t* >( m_ntos ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x8 ] != 0xec
           || ctx[ 0xb ] != 0x89
           || ctx[ 0xc ] != 0x7c
           || ctx[ 0xf ] != 0x48 )
         ctx++;

      std::ptrdiff_t process{ };

      ptr< std::int32_t( __stdcall* )(
         std::ptrdiff_t handle,
         std::ptrdiff_t* process
      ) >( ctx )( 0x4, &process );

      return process;
   }

   [[ nodiscard ]]
   const std::ptrdiff_t get_thread( ) {
      auto ctx{ ptr< std::uint8_t* >( m_ntos ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x3 ] != 0x04
           || ctx[ 0x4 ] != 0x25
           || ctx[ 0x9 ] != 0xc3 )
         ctx++;
  
      return ptr< std::ptrdiff_t( __stdcall* )( ) >( ctx )( );
   }

   [[ nodiscard ]]
   const std::ptrdiff_t get_cid_entry( ) {
      auto ctx{ ptr< std::uint8_t* >( m_ntos ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x0 ] != 0x8b
           || ctx[ 0x1 ] != 0x01
           || ctx[ 0x2 ] != 0x48
           || ctx[ 0x3 ] != 0x83
           || ctx[ 0x4 ] != 0xe2 )
         ctx++;

      auto thread{ *ptr< std::ptrdiff_t* >( get_thread( ) + 0x650 ) };
      if ( !thread )
         return 0;

      return ptr< std::ptrdiff_t( __fastcall* )(
         std::ptrdiff_t cid_table,
         std::ptrdiff_t thread_id
      ) >( ctx )( get_cid_table( ), thread );
   }

   const std::uint8_t unlink_handle( ) {
      auto id{ *ptr< std::ptrdiff_t* >( get_thread( ) + 0x650 ) };
      if ( !id )
         return 0;

      auto ctx{ ptr< std::uint8_t* >( m_ntos ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x0 ] != 0xe8
           || ctx[ 0x5 ] != 0x48
           || ctx[ 0x6 ] != 0x8b
           || ctx[ 0x7 ] != 0xce
           || ctx[ 0x8 ] != 0xe8
           || ctx[ 0xd ] != 0x4c
           || ctx[ 0xe ] != 0x39 )
         ctx++;

      auto cast{ *ptr< std::int32_t* >( &ctx[ 0x1 ] ) };
      if ( !cast )
         return 0;

      return !ptr< std::int32_t( __stdcall* )(
         std::ptrdiff_t cid_table,
         std::ptrdiff_t thread_id,
         std::ptrdiff_t cid_entry
      ) >( &ctx[ 0x5 ] + cast )( get_cid_table( ), id, get_cid_entry( ) );
   }

   const std::uint8_t clean_mdl_pfn( ) {

      struct mdl_page_t {
         std::uint8_t m_pad0[ 0x20 ];
         std::ptrdiff_t m_mapped_va;
         std::uint32_t m_byte_count, m_byte_offset;
      };

      auto mdl{ ptr< mdl_page_t* >( m_pmdl ) };
      if ( !mdl )
         return 0;

      auto page_ratio{ ( mdl->m_mapped_va + mdl->m_byte_offset ) & 0xfff };
      auto page_count{ ( page_ratio + mdl->m_byte_count + 0xfff ) >> 0xc };

      for ( std::size_t i{ }; i < page_count; i++ )
         ptr< std::ptrdiff_t* >( m_pmdl + 0x1 )[ i ] = 0;

      return 1;
   }

   const std::uint8_t stack_attach(
      const std::ptrdiff_t process,
      const std::ptrdiff_t apc_stub
   ) {
      auto ctx{ ptr< std::uint8_t* >( m_ntos ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x2a ] != 0xf7
           || ctx[ 0x2b ] != 0x81
           || ctx[ 0x2c ] != 0xb8 )
         ctx++;

      return !ptr< std::int32_t( __stdcall* )( 
         std::ptrdiff_t process,
         std::ptrdiff_t apc_stub
      ) >( ctx )( process, apc_stub );
   }

   const std::uint8_t stack_detach(
      const std::ptrdiff_t apc_stub
   ) {
      auto ctx{ ptr< std::uint8_t* >( m_ntos ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x0 ] != 0xe8
           || ctx[ 0x5 ] != 0x33
           || ctx[ 0x6 ] != 0xdb
           || ctx[ 0x7 ] != 0xeb
           || ctx[ 0x8 ] != 0xab )
         ctx++;

      auto cast{ *ptr< std::int32_t* >( &ctx[ 0x1 ] ) };
      if ( !cast )
         return 0;

      return !ptr< std::int32_t( __stdcall* )(
         std::ptrdiff_t apc_stub
      ) >( &ctx[ 0x5 ] + cast )( apc_stub );
   }

   const std::uint8_t unlink_list(
      const std::ptrdiff_t entry
   ) {
      struct entry_t {
         entry_t* m_next;
         entry_t* m_prev; 
      };

      auto ctx{ ptr< entry_t* >( entry ) };
      if ( !ctx )
         return 0;

      ctx->m_next->m_prev = ctx->m_prev;
      ctx->m_prev->m_next = ctx->m_next;
      
      ctx->m_next = ctx;
      ctx->m_prev = ctx;

      return 1;
   }

   const std::uint8_t unlink_thread( ) {
      auto flags{ *ptr< std::uint32_t* >( get_thread( ) + 0x74 ) };
      if ( !flags )
         return 0;

      flags &= ( 0x0 << 0x04 ); // alertable
      flags &= ( 0x0 << 0x06 ); // interrupt
      flags &= ( 0x0 << 0x0e ); // systhread
      flags &= ( 0x0 << 0x0a ); // apcqueue
      flags &= ( 0x0 << 0x10 ); // shadowstack

      *ptr< std::int64_t* >( get_thread( ) + 0x030 ) = 0; // stacklimit
      *ptr< std::int64_t* >( get_thread( ) + 0x038 ) = 0; // stackbase
      *ptr< std::int64_t* >( get_thread( ) + 0x058 ) = 0; // kernelstack
      *ptr< std::int64_t* >( get_thread( ) + 0x6a0 ) = 0; // win32startaddress

      *ptr< std::int64_t* >( get_thread( ) + 0x620 ) = 0; // startaddress
      *ptr< std::int64_t* >( get_thread( ) + 0x648 ) = 0; // uniqueprocess
      *ptr< std::int64_t* >( get_thread( ) + 0x650 ) = 0; // uniquethread
      *ptr< std::int64_t* >( get_thread( ) + 0x724 ) = 0; // stackreference
      *ptr< std::int64_t* >( get_thread( ) + 0x1e4 ) = 1; // combinedisable

      unlink_list( get_thread( ) + 0x2f8 ); // kthreadlist
      unlink_list( get_thread( ) + 0x6b8 ); // ethreadlist
      unlink_list( get_thread( ) + 0x0d8 ); // waitlist
      unlink_list( get_thread( ) + 0x608 ); // waitchain

      unlink_list( get_thread( ) + 0x680 ); // irplist
      unlink_list( get_thread( ) + 0x7f8 ); // ownerlist
      unlink_list( get_thread( ) + 0x810 ); // disownedlist

      return 1;
   }

   template< typename... arg_t >
   const std::uint8_t msg(
      const std::string_t msg,
      arg_t... args
   ) {
      auto ctx{ ptr< std::uint8_t* >( m_ntos ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x2 ] != 0xdc
           || ctx[ 0x3 ] != 0x4d
           || ctx[ 0x4 ] != 0x89
           || ctx[ 0x5 ] != 0x43
           || ctx[ 0x6 ] != 0x18
           || ctx[ 0x7 ] != 0x4d )
         ctx++;

      return !ptr< std::int32_t( __stdcall* )(
         std::int32_t id,
         std::int32_t level,
         std::string_t msg,
         arg_t... variadic
      ) >( ctx )( 0, 0, msg, args... );
   }

   [[ nodiscard ]]
   const std::uint8_t end_thread( ) {
      auto ctx{ ptr< std::uint8_t* >( m_ntos ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x10 ] != 0x41
           || ctx[ 0x16 ] != 0x0f
           || ctx[ 0x17 ] != 0x84
           || ctx[ 0x1c ] != 0x41
           || ctx[ 0x1d ] != 0xb0 )
         ctx++;

      return !ptr< std::int32_t( __stdcall* )(
         std::int32_t exit_code
      ) >( ctx )( 0 );
   }

   [[ nodiscard ]]
   const std::ptrdiff_t new_thread(
      const auto callback
   ) {
      auto ctx{ ptr< std::uint8_t* >( m_ntos ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x4e ] != 0x54 
           || ctx[ 0x4f ] != 0x24
           || ctx[ 0x50 ] != 0x70
           || ctx[ 0x51 ] != 0x33
           || ctx[ 0x52 ] != 0xd2
           || ctx[ 0x53 ] != 0x48 )
         ctx++;

      auto cast{ ptr< std::ptrdiff_t >( callback ) };
      if ( !cast )
         return 0;

      std::ptrdiff_t handle{ };

      ptr< std::int32_t( __stdcall* )(
         std::ptrdiff_t* thread,
         std::int32_t access,
         std::ptrdiff_t attributes,
         std::ptrdiff_t process,
         std::ptrdiff_t client,
         std::ptrdiff_t routine,
         std::ptrdiff_t context,
         std::ptrdiff_t unused,
         std::ptrdiff_t reserved
      ) >( ctx )( &handle, 0, 0, 0, 0, cast, 0, 0, 0 );

      return handle;
   }

   const std::uint8_t close(
      const std::ptrdiff_t handle
   ) {
      auto ctx{ ptr< std::uint8_t* >( m_ntos ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x4 ] != 0x45
           || ctx[ 0x5 ] != 0x3b
           || ctx[ 0x6 ] != 0x26
           || ctx[ 0x7 ] != 0x1b )
         ctx++;

      auto cast{ *ptr< std::int32_t* >( &ctx[ 0x0 ] ) };
      if ( !cast )
         return 0;

      return !ptr< std::int32_t( __stdcall* )(
         std::ptrdiff_t handle
      ) >( &ctx[ 0x4 ] + cast )( handle );
   }

   const std::uint8_t delete_lookaside(
      const std::ptrdiff_t lookaside_list
   ) {
      auto ctx{ ptr< std::uint8_t* >( m_ntos ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x0d ] != 0x01
           || ctx[ 0x0e ] != 0x48
           || ctx[ 0x0f ] != 0x8d
           || ctx[ 0x10 ] != 0x0d
           || ctx[ 0x15 ] != 0x48 )
         ctx++;

      return !ptr< std::int32_t( __stdcall* )(
         std::ptrdiff_t lookaside_list
      ) >( ctx )( lookaside_list );
   }

   const std::uint8_t create_lookaside(
      const std::ptrdiff_t lookaside_list,
      const std::uint32_t type,
      const std::size_t size,
      const std::uint32_t tag
   ) {
      auto ctx{ ptr< std::uint8_t* >( m_ntos ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x4 ] != 0x83 
           || ctx[ 0x5 ] != 0x64
           || ctx[ 0x6 ] != 0x24
           || ctx[ 0x9 ] != 0x0f )
         ctx++;

      return !ptr< std::int32_t( __stdcall* )(
         std::ptrdiff_t lookaside_list,
         std::ptrdiff_t alloc_fn,
         std::ptrdiff_t free_fn,
         std::uint32_t pool_type,
         std::uint32_t flags,
         std::size_t size,
         std::uint32_t tag,
         std::uint16_t depth
      ) >( ctx )( lookaside_list, 0, 0, type, 0, size, tag, 0 );
   }

   const std::uint8_t clean_bigpool( ) {
      auto ctx{ ptr< std::uint8_t* >( m_cint ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x12 ] != 0x24 
           || ctx[ 0x18 ] != 0x33
           || ctx[ 0x19 ] != 0xd2
           || ctx[ 0x1a ] != 0x48 )
         ctx++;

      auto cast{ *ptr< std::int32_t* >( &ctx[ 0x1 ] ) };
      if ( !cast )
         return 0;

      struct lookaside_stub_t {
         std::uint8_t m_pad0[ 0x24 ];
         std::uint32_t m_type, m_tag, m_size;
         std::uint8_t m_pad1[ 0x30 ];
      };

      auto list{ ptr< lookaside_stub_t* >( &ctx[ 0x5 ] + cast ) };
      if ( !list )
         return 0;

      delete_lookaside( ptr< >( list ) );
      create_lookaside( ptr< >( list ), list->m_type, list->m_size, list->m_tag );

      return !!list;
   }

   [[ nodiscard ]]
   const std::ptrdiff_t wstring_find(
      const std::wstring_t wstring,
      const std::wstring_t subwstring
   ) {
      auto ctx{ ptr< std::uint8_t* >( m_ntos ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0xb ] != 0x39
           || ctx[ 0xc ] != 0x1a
           || ctx[ 0xe ] != 0x04
           || ctx[ 0xf ] != 0x48 )
         ctx++;

      if ( !wstring || !subwstring )
         return 0;

      return ptr< std::ptrdiff_t( __stdcall* )(
         const std::wstring_t wstring,
         const std::wstring_t subwstring
      ) >( ctx )( wstring, subwstring );
   }

   [[ nodiscard ]]
   const std::ptrdiff_t process_by_name(
      const std::wstring_t process_name
   ) {
      constexpr std::uint32_t pe_link{ 0x640 };
      constexpr std::uint32_t pe_path{ 0x468 };

      for ( auto ctx{ get_initial_process( ) }; ctx; ) {

         struct unicode_stub_t {
            std::uint16_t m_length, m_capacity;
            std::wstring_t m_buffer;
         };

         if ( auto uni{ *ptr< unicode_stub_t** >( ctx + pe_path ) }; uni )
            if ( wstring_find( uni->m_buffer, process_name ) )
               return ctx;

         ctx = *ptr< std::ptrdiff_t* >( ctx + pe_link ) - pe_link;
      }
      return 0;
   }

   //[[ nodiscard ]]
   const std::ptrdiff_t module_by_name(
      const std::ptrdiff_t process,
      const std::wstring_t module_name
   ) {
      constexpr std::uint32_t pe_peb{ 0x3f8 };
      constexpr std::uint32_t pe_ldr{ 0x018 };

      auto peb{ *ptr< std::ptrdiff_t* >( process + pe_peb ) };
      if ( !peb )
         return 0;

      auto ldr{ *ptr< std::ptrdiff_t* >( peb + pe_ldr ) };
      if ( !ldr )
         return 0;

      struct entry_t {
         entry_t* m_next;
         entry_t* m_prev;
      };

      {
         auto ctx{ ptr< entry_t* >( ldr + 0x10 ) };
         if ( !ctx )
            return 0;
         unsigned long b = 0;
         do {
            b++;
            msg( "--> entry %llx\n", ctx );
            ctx = ctx->m_next;
         } while ( ctx != ptr< entry_t* >( ldr + 0x10 ) );
         msg("**** queried %lx\n\n", b);
      }

      {
         auto ctx{ ptr< entry_t* >( ldr + 0x20 ) };
         if ( !ctx )
            return 0;
         unsigned long b = 0;
         do {
            b++;
            msg( "--> entry %llx\n", ctx );
            ctx = ctx->m_next;
         } while ( ctx != ptr< entry_t* >( ldr + 0x20 ) );
         msg("**** queried %lx\n\n", b);
      }

      {
         auto ctx{ ptr< entry_t* >( ldr + 0x30 ) };
         if ( !ctx )
            return 0;
         unsigned long b = 0;
         do {
            b++;
            msg( "--> entry %llx\n", ctx );
            ctx = ctx->m_next;
         } while ( ctx != ptr< entry_t* >( ldr + 0x30 ) );
         msg("**** queried %lx\n\n", b);
      }

      ( module_name );

      return 0;
   }

   std::ptrdiff_t m_ntos;
   std::ptrdiff_t m_pmdl;
   std::ptrdiff_t m_cint;
};

extern kernel_t* kernel;