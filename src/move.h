/*
 * move.h
 *
 *  Created on: 12 août 2018
 *      Author: Bertrand
 */

#ifndef SRC_MOVE_H_
#define SRC_MOVE_H_

  /**
   * @enum Flag
   * @brief Flags that indicate special moves.
   */
  enum Flag {
    NULL_MOVE = 1<<0, /**< The move is a null move */
    CAPTURE = 1<<1, /**< The move is a capture (Captured piece type must be set with setCapturedPieceType()) */
    DOUBLE_PAWN_PUSH = 1<<2, /**< The move is a double pawn push */
    KSIDE_CASTLE = 1<<3, /**< The move is a kingisde castle */
    QSIDE_CASTLE = 1<<4, /**< The move is a queenside castle */
    EN_PASSANT = 1<<5, /**< The move is an en passant capture (Do not set the CAPTURE flag additionally) */
    PROMOTION = 1<<6 /**< The move is a promotion (Promotion piece type must be set with setPromotionPieceType()) */
  };


  /**
   * @brief A packed integer containing all of the move data.
   *
   * 28 bits are used in total to store move information. The format is as
   * follows:
   *
   * @code
   * MSB  |----6----|---5---|---4---|-3--|-2--|-1--|  LSB
   *      28        21      15      9    6    3    0
   * @endcode
   *
   * - 6 - Flags (7 bits)
   * - 5 - To square (6 bits)
   * - 4 - From square (6 bits)
   * - 3 - Captured PieceType (if applicable) (3 bits)
   * - 2 - Promotion PieceType (if applicable) (3 bits)
   * - 1 - PieceType of moving piece (3 bits)
   *
   * Moves are stored as a packed integer consisting of 28 bits total.
   */
  unsigned int _move;

  /** @brief Value of this move */
  int _value;

  /**
   * @name Rank and file characters in algebraic notation
   *
   * @{
   */
  const static char RANKS[];
  const static char FILES[];
  /**@}*/



#endif /* SRC_MOVE_H_ */
