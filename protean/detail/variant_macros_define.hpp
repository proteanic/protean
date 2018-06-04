#ifndef CHECK_VARIANT_FUNCTION
#define CHECK_VARIANT_FUNCTION(TYPE, FUNCTION_CALL)                                                                                                 \
    if (!is<TYPE>())                                                                                                                                \
    {                                                                                                                                               \
        boost::throw_exception(variant_error(std::string("Attempt to call ") + FUNCTION_CALL + " on " + enum_to_string(m_type) + " variant"));      \
    }
#endif // CHECK_VARIANT_FUNCTION

#ifndef BEGIN_TRANSLATE_ERROR
#define BEGIN_TRANSLATE_ERROR                                                                           \
        try                                                                                             \
        {                                                                                               \
          void
#endif // BEGIN_TRANSLATE_ERROR

#ifndef END_TRANSLATE_ERROR
#define END_TRANSLATE_ERROR                                                                             \
        }                                                                                               \
        catch(std::exception& e)                                                                        \
        {                                                                                               \
            boost::throw_exception(variant_error(std::string(e.what()) + "\n" + this->str()));          \
        }                                                                                               \
        catch(...)                                                                                      \
        {                                                                                               \
            boost::throw_exception(variant_error(std::string("Unknown exception\n") + this->str()));    \
        }                                                                                               \
        void
#endif // END_TRANSLATE_ERROR
