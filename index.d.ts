export class Oodle {
  constructor(state: Buffer);

  /**
   * Decode a packet
   *
   * @return The output.
   */
  decode(input: Buffer, outputLen: Number): Buffer;
}
