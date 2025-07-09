module SnocList where

import Control.Applicative

newtype SnocList a = SnocList {unSnocList :: [a]}

toList :: SnocList a -> [a]
toList (SnocList xs) = reverse xs

fromList :: [a] -> SnocList a
fromList xs = SnocList (reverse xs)

snoc :: SnocList a -> a -> SnocList a
snoc (SnocList xs) x = SnocList (x : xs)

instance Eq a => Eq (SnocList a) where
  a == b = toList a == toList b

instance Show a => Show (SnocList a) where
  show = show . toList

instance Semigroup (SnocList a) where
  SnocList xs <> SnocList ys = SnocList (ys ++ xs)

instance Monoid (SnocList a) where
  mempty = SnocList []
  mappend = (<>)

instance Functor SnocList where
  fmap f (SnocList xs) = SnocList (map f xs)

instance Applicative SnocList where
  pure x = SnocList [x]
  SnocList fs <*> SnocList xs = fromList [f x | f <- reverse fs, x <- reverse xs]

instance Alternative SnocList where
  empty = mempty
  (<|>) = (<>)