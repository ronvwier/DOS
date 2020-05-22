typedef void TAAKPROC(void);

class TAAK
{
    public:
    start(TAAKPROC *);
    stop(void);
    run(void);

    private:
    TAAKPROC * procedure;
};

