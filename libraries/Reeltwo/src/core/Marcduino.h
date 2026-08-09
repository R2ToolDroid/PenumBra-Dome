#ifndef Marcduino_h
#define Marcduino_h

#include "ReelTwo.h"
#include "core/Animation.h"
#include "core/JawaCommander.h"

#define MARCDUINO_ANIMATION(name, marc) \
    ANIMATION_FUNC_DECL(name); \
    const char _marc_msg_##name[] PROGMEM = #marc; \
    Marcduino Marc_##name(Animation_##name, _marc_msg_##name); \
    ANIMATION(name)

#define MARCDUINO_ACTION(name, marc, p) \
    MARCDUINO_ANIMATION(name, marc) \
    { \
        DO_START() \
        DO_ONCE(p) \
        DO_END() \
    }

class Marcduino
{
public:
    Marcduino(AnimationStep animation, const char* marc /* PROGMEM */) :
        fMarc(marc),
        fAnimation(animation),
        fNext(NULL)
    {
        if (*head() == NULL)
            *head() = this;
        if (*tail() != NULL)
            (*tail())->fNext = this;
        *tail() = this;
    }

    static void processCommand(AnimationPlayer& player, const char* cmd)
    {
#if defined(DEBUG_SERIAL) || defined(USE_DEBUG)
        // Log the incoming raw command for diagnostics
        Serial.print("[MARCDUINO PROC] '");
        Serial.print(cmd);
        Serial.println("'");
#endif

        // Find the best (longest) matching registered command to avoid
        // shorter prefixes shadowing longer ones (e.g. ':SE0' vs ':SE00').
        Marcduino* best = NULL;
        int bestLen = 0;
        bool bestIsAt = false;

        for (Marcduino* marc = *head(); marc != NULL; marc = marc->fNext)
        {
            int plen = strlen_P(marc->fMarc);
            // normal match
            if (strncmp_P(cmd, marc->fMarc, plen) == 0)
            {
                if (plen > bestLen)
                {
                    best = marc;
                    bestLen = plen;
                    bestIsAt = (pgm_read_byte(marc->fMarc) == '@');
                }
            }
            else if (pgm_read_byte(marc->fMarc) == '@' && plen > 1)
            {
                // special '@' pattern: match digits in cmd to the rest of the pattern
                // marc->fMarc like '@XYZ' means accept a leading digit and then match XYZ
                // compare marc->fMarc+1 (length plen-1)
                if (isdigit(cmd[0]) && strncmp_P(cmd, marc->fMarc+1, plen-1) == 0)
                {
                    if (plen > bestLen)
                    {
                        best = marc;
                        bestLen = plen; // keep plen (including '@') for precedence
                        bestIsAt = true;
                    }
                }
            }
        }

        bool found = false;
        if (best != NULL)
        {
            int plen = strlen_P(best->fMarc);
            int matchLen = plen;
            if (pgm_read_byte(best->fMarc) == '@' && plen > 0)
            {
                // for '@' patterns the actual consumed prefix length for cmd->rest
                // is (plen - 1) when '@' denotes a variable digit, but the
                // registration length includes the '@' so adjust accordingly.
                matchLen = plen - 1;
            }

#if defined(DEBUG_SERIAL) || defined(USE_DEBUG)
            // Log which command matched and the remaining args
            Serial.print("[MARCDUINO PROC] Matched rest='");
            Serial.print(cmd + matchLen);
            Serial.println("'");
#endif

            *command() = cmd + matchLen;
            AnimationStep animation = best->fAnimation;
            if (animation != NULL)
            {
                player.animateOnce(animation);
                found = true;
            }
        }

        // Check for unprocessed Jawa lite command if not found
        if (!found && *cmd == '@')
        {
            JawaCommanderBase* base = JawaCommanderBase::get();
            if (base != NULL)
            {
                base->process(cmd+1);
                found = true;
            }
        }

#if defined(DEBUG_SERIAL) || defined(USE_DEBUG)
        if (!found)
        {
            Serial.print("[MARCDUINO PROC] NotFound '");
            Serial.print(cmd);
            Serial.println("'");
        }
#endif
    } 

    static void send(PROGMEMString cmd)
    {
#ifndef USE_SMQ
        UNUSED_ARG(cmd)
#else
        if (SMQ::sendTopic("MARC"))
        {
            SMQ::send_string(F("cmd"), cmd);
            SMQ::send_end();
        }
#endif
    }

    static void send(const char* cmd)
    {
#ifndef USE_SMQ
        UNUSED_ARG(cmd)
#else
        if (SMQ::sendTopic("MARC"))
        {
            SMQ::send_string(F("cmd"), cmd);
            SMQ::send_end();
        }
#endif
    }

    static const char* getCommand()
    {
        return *command();
    }

private:
    const char* fMarc;
    AnimationStep fAnimation;
    Marcduino* fNext;

    static const char** command()
    {
        static const char* sCmd;
        return &sCmd;
    }

    static Marcduino** head()
    {
        static Marcduino* sHead;
        return &sHead;
    }

    static Marcduino** tail()
    {
        static Marcduino* sTail;
        return &sTail;
    }
};

// MarcduinoSerial unchanged from previous debug instrumentation version

template<uint16_t BUFFER_SIZE=64> class MarcduinoSerial : public AnimatedEvent
{
public:
    MarcduinoSerial(HardwareSerial &serial, AnimationPlayer &player) :
        fStream(&serial),
        fPlayer(player),
        fPos(0)
    {
#if defined(DEBUG_SERIAL) || defined(USE_DEBUG)
        // Enable debug echo when requested
        fOutStream = &Serial;
#else
        fOutStream = nullptr;
#endif
    }

    MarcduinoSerial(Stream* stream, AnimationPlayer &player) :
        fStream(stream),
        fPlayer(player),
        fPos(0)
    {
#if defined(DEBUG_SERIAL) || defined(USE_DEBUG)
        fOutStream = &Serial;
#else
        fOutStream = nullptr;
#endif
    }

    MarcduinoSerial(AnimationPlayer &player) :
        fStream(nullptr),
        fPlayer(player),
        fPos(0)
    {
#if defined(DEBUG_SERIAL) || defined(USE_DEBUG)
        fOutStream = &Serial;
#else
        fOutStream = nullptr;
#endif
    }

    void setStream(Stream* stream, Stream* outStream = nullptr)
    {
        fStream = stream;
        fOutStream = outStream;
    }

    virtual void animate()
    {
        if (fStream == nullptr) return;

        // Read and process all available bytes in one go to avoid
        // inter-byte delays causing dropped or partial buffers.
        while (fStream->available())
        {
            int ch = fStream->read();
            if (ch == -1) break;

#if defined(DEBUG_SERIAL) || defined(USE_DEBUG)
            // Log every received byte (hex + ASCII when printable)
            Serial.print("RXBYTE 0x");
            if (ch < 16) Serial.print('0');
            Serial.print(ch, HEX);
            Serial.print(" '");
            if (ch >= 32 && ch <= 126)
                Serial.print((char)ch);
            else if (ch == '\r')
                Serial.print("\\r");
            else if (ch == '\n')
                Serial.print("\\n");
            else
                Serial.print('.');
            Serial.println("'");
#endif

            // Echo incoming byte to debug stream if set, but only if there's buffer
            // space to avoid blocking and possible receiver starvation.
            if (fOutStream != nullptr)
            {
#if defined(HAVE_STREAM_AVAILABLEFORWRITE)
                if (fOutStream->availableForWrite && fOutStream->availableForWrite() > 0)
                {
                    uint8_t buf = (uint8_t)ch;
                    fOutStream->write(&buf, 1);
                }
#else
                // Fallback: attempt non-blocking write if possible (best-effort)
                uint8_t buf = (uint8_t)ch;
                fOutStream->write(&buf, 1);
#endif
            }

            // Accept CR or LF as terminator. This handles CR, LF, and CR+LF
            if (ch == '\r' || ch == '\n')
            {
#if defined(DEBUG_SERIAL) || defined(USE_DEBUG)
                fBuffer[fPos] = '\0';

                // Log received buffer on terminator
                Serial.print("[MARCDUINO RX] '");
                Serial.print(fBuffer);
                Serial.println("'");
#endif

                fPos = 0;
                if (fBuffer[0] != '\0')
                {
                    Marcduino::processCommand(fPlayer, fBuffer);
                }
            }
            else if (fPos < SizeOfArray(fBuffer)-1)
            {
                fBuffer[fPos++] = ch;
            }
            else
            {
                // Buffer overflow protection: drop current contents and log
                fPos = 0;
#if defined(DEBUG_SERIAL) || defined(USE_DEBUG)
                Serial.println("[MARCDUINO RX] Buffer overflow - dropping data");
#endif
            }
        }
    }

private:
    Stream* fStream;
    Stream* fOutStream = nullptr;
    AnimationPlayer& fPlayer;
    char fBuffer[BUFFER_SIZE];
    unsigned fPos;
};

#endif
