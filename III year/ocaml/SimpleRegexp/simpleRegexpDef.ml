(** Składnia abstrakcyjna wyrażeń regularnych nad (abstrakcyjnym) alfabetem ['c]. *)

type 'c reg =
  | Lit of 'c                 (** słowo jednoliterowe *)
  | Concat of 'c reg * 'c reg (** konkatenacja *)
  | Or of 'c reg * 'c reg     (** alternatywa (suma) *)
  | Star of 'c reg            (** powtórzenie *)
  | Eps                       (** słowo puste *)
  | Empty                     (** język pusty *)
