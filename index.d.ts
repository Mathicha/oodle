export class Oodle {
  constructor(state: Buffer);

  /**
   * Decode a packet
   *
   * The buffers must be preallocated.
   *
   * @return The output itself.
   */
  decode(input: Buffer, output: Buffer): Buffer;
}
