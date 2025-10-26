class Dir
{
public:
	Dir(const char *path);
	~Dir();

	const char *read();

	operator bool() const;

private:
	void *m_dir;
};
