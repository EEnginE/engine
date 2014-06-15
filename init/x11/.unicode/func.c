long keysym2unicode( KeySym keysym ) {
   int min = 0;
   int max = sizeof( keysymtab ) / sizeof( struct codepair ) - 1;
   int mid;

   /* first check for Latin-1 characters (1:1 mapping) */
   if ( ( keysym >= 0x0020 && keysym <= 0x007e ) ||
         ( keysym >= 0x00a0 && keysym <= 0x00ff ) )
      return keysym;

   /* also check for directly encoded 24-bit UCS characters */
   if ( ( keysym & 0xff000000 ) == 0x01000000 )
      return keysym & 0x00ffffff;

   /* binary search in table */
   while ( max >= min ) {
      mid = ( min + max ) / 2;
      if ( keysymtab[mid].keysym < keysym )
         min = mid + 1;
      else if ( keysymtab[mid].keysym > keysym )
         max = mid - 1;
      else {
         /* found it */
         return keysymtab[mid].ucs;
      }
   }

   /* no matching Unicode value found */
   return -1;
}
