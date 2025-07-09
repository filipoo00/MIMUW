module type REGEXP = sig

  (** Typ wyrażeń regularnych *)
  type t

  (** Parser powinien być w stanie sparsować wyrażenia regularne używające 
      składni obowiązującej w zadaniu. *)
  val re: string -> t

  (** Drukowanie reprezentacji wyrażeń regularnych na stdout; 
      na końcu powinno być przejście do nowego wiersza. *)
  val debug: t -> unit

  (** Sprawdzanie czy {b cały} napis pasuje do wyrażenia regularnego. *)
  val matches: t -> string -> bool

end