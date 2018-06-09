#pragma once

#include "DiscreteSoundNode.h"

/************************************************************************
 *
 * DST_MIXER  - Mixer/Gain stage
 *
 * input[0]    - Enable input value
 * input[1]    - Input 1
 * input[2]    - Input 2
 * input[3]    - Input 3
 * input[4]    - Input 4
 * input[5]    - Input 5
 * input[6]    - Input 6
 * input[7]    - Input 7
 * input[8]    - Input 8
 *
 * Also passed discrete_mixer_info structure
 *
 * Mar 2004, D Renaud.
 ************************************************************************/
/*
 * The input resistors can be a combination of static values and nodes.
 * If a node is used then its value is in series with the static value.
 * Also if a node is used and its value is 0, then that means the
 * input is disconnected from the circuit.
 *
 * There are 3 basic types of mixers, defined by the 2 types.  The
 * op amp mixer is further defined by the prescence of rI.  This is a
 * brief explaination.
 *
 * DISC_MIXER_IS_RESISTOR
 * The inputs are high pass filtered if needed, using (rX || rF) * cX.
 * Then Millman is used for the voltages.
 * r = (1/rF + 1/r1 + 1/r2...)
 * i = (v1/r1 + v2/r2...)
 * v = i * r
 *
 * DISC_MIXER_IS_OP_AMP - no rI
 * This is just a summing circuit.
 * The inputs are high pass filtered if needed, using rX * cX.
 * Then a modified Millman is used for the voltages.
 * i = ((vRef - v1)/r1 + (vRef - v2)/r2...)
 * v = i * rF
 *
 * DISC_MIXER_IS_OP_AMP_WITH_RI
 * The inputs are high pass filtered if needed, using (rX + rI) * cX.
 * Then Millman is used for the voltages including vRef/rI.
 * r = (1/rI + 1/r1 + 1/r2...)
 * i = (vRef/rI + v1/r1 + v2/r2...)
 * The voltage is then modified by an inverting amp formula.
 * v = vRef + (rF/rI) * (vRef - (i * r))
 */

class discrete_device //: public device_t
{
	//friend class discrete_base_node;

protected:
	// construction/destruction
//	discrete_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock);

public:
//	// inline configuration helpers
//	static void static_set_intf(device_t &device, const discrete_block *intf);
//
//	DECLARE_READ8_MEMBER(read);
//	DECLARE_WRITE8_MEMBER(write);
//	virtual ~discrete_device(void);
//
//	/* --------------------------------- */
//
	virtual void update_to_current_time(void) const {  }
//
//	/* process a number of samples */
//	void process(int samples);
//
//	/* access to the discrete_logging facility */
//	void CLIB_DECL ATTR_PRINTF(2,3) discrete_log(const char *text, ...) const;
//
//	/* get pointer to a info struct node ref */
	const double *node_output_ptr(int onode);
//
//	/* FIXME: this is used by csv and wav logs - going forward, identifiers should be explicitly passed */
//	int same_module_index(const discrete_base_node &node);
//
//	/* get node */
//	discrete_base_node *discrete_find_node(int node);
//
//	/* are we profiling */
//	inline int profiling(void) { return m_profiling; }
//
	inline int sample_rate() { return m_sample_rate; }
	inline double sample_time() { return m_sample_time; }
//
//
//protected:
//
//	// device-level overrides
//	virtual void device_start();
//	virtual void device_reset();
//	virtual void device_stop();
//
//	// configuration state
//	const discrete_block *m_intf;
//
//	// internal state
//
//	/* --------------------------------- */
//
//	/* emulation info */
	int					m_sample_rate;
	double				m_sample_time;
//	double				m_neg_sample_time;
//
//	/* list of all nodes */
//	node_list_t				m_node_list;		/* node_description * */
//
//private:
//	void discrete_build_list(const discrete_block *intf, sound_block_list_t &block_list);
//	void discrete_sanity_check(const sound_block_list_t &block_list);
//	void display_profiling(void);
//	void init_nodes(const sound_block_list_t &block_list);
//
//	/* internal node tracking */
//	discrete_base_node **	m_indexed_node;
//
//	/* tasks */
//	task_list_t				task_list;		/* discrete_task_context * */
//
//	/* debugging statistics */
//	FILE *					m_disclogfile;
//
//	/* parallel tasks */
//	osd_work_queue *		m_queue;
//
//	/* profiling */
//	int 					m_profiling;
//	UINT64					m_total_samples;
//	UINT64					m_total_stream_updates;
};

class discrete_base_node
{
	friend class discrete_device;
	template <class C> friend class discrete_node_factory;
	friend class discrete_task;

public:

	virtual void reset(void) { }
	virtual void start(void) { }
	virtual void stop(void) { }
	virtual void save_state(void);

	virtual int max_output(void) { return 1; };

//	inline bool interface(discrete_step_interface *&intf) const { intf = m_step_intf; return (intf != NULL); }
//	inline bool interface(discrete_input_interface *&intf) const { intf = m_input_intf; return (intf != NULL); }
//	inline bool interface(discrete_sound_output_interface *&intf) const { intf = m_output_intf; return (intf != NULL); }

	/* get the input value from node #n */
	inline double input(int n) { return *(m_input[n]); }

	/* set an output */
	inline void set_output(int n, double val) { m_output[n] = val; }

	///* Return the node index, i.e. X from NODE(X) */
	//inline int index(void) { return NODE_INDEX(m_block->node); }

	///* Return the node number, i.e. NODE(X) */
	//inline int block_node(void) const { return m_block->node;  }

	///* Custom function specific initialisation data */
	//inline const void *custom_data(void) { return m_custom; }

	//inline int input_node(int inputnum) { return m_block->input_node[inputnum]; }

	/* Number of active inputs on this node type */
	inline int			active_inputs(void) { return m_active_inputs; }
	///* Bit Flags.  1 in bit location means input_is_node */
	//inline int			input_is_node(void) { return m_input_is_node; }

	inline double		sample_time(void) { return m_device->sample_time(); }
	inline int			sample_rate(void) { return m_device->sample_rate(); }

	//const char *		module_name(void) { return m_block->mod_name; }
	//inline int			module_type(void) const { return m_block->type; }

protected:

	discrete_base_node();
	virtual ~discrete_base_node();

	/* finish node setup after allocation is complete */
//	void init(discrete_device * pdev, const discrete_block *block);

	void resolve_input_nodes(void);

	double							m_output[DISCRETE_MAX_OUTPUTS];		/* The node's last output value */
	const double *					m_input[DISCRETE_MAX_INPUTS];		/* Addresses of Input values */
	discrete_device *				m_device;							/* Points to the parent */

private:

//	const discrete_block *	m_block;							/* Points to the node's setup block. */
	int								m_active_inputs;					/* Number of active inputs on this node type */

	const void *					m_custom;							/* Custom function specific initialisation data */
	int								m_input_is_node;

//	discrete_step_interface *		m_step_intf;
//	discrete_input_interface *		m_input_intf;
//	discrete_sound_output_interface *		m_output_intf;
};

//
namespace Emulator
{

#define DST_MIXER__IN(bit)		DISCRETE_INPUT(bit + 1)

#define DISC_MIXER_MAX_INPS	8
#define DISC_MAX_MIXER_INPUTS	8

/* Mixer types */
#define DISC_MIXER_IS_RESISTOR				0
#define DISC_MIXER_IS_OP_AMP				1
#define DISC_MIXER_IS_OP_AMP_WITH_RI		2	/* Used only internally.  Use DISC_MIXER_IS_OP_AMP */

class Mixer : public DiscreteSoundNode
{ 
public:
	struct Desc
	{
		int		type;
		double	r[DISC_MAX_MIXER_INPUTS];		/* static input resistance values.  These are in series with rNode, if used. */
		int		r_node[DISC_MAX_MIXER_INPUTS];	/* variable resistance nodes, if needed.  0 if not used. */
		double	c[DISC_MAX_MIXER_INPUTS];
		double	rI;
		double	rF;
		double	cF;
		double	cAmp;
		double	vRef;
		double	gain;				/* Scale value to get output close to +/- 32767 */
	};

	Mixer(DiscreteDevice* pDevice, const Desc& desc) :
		DiscreteSoundNode(pDevice),
		m_desc(desc)
	{
	}
	virtual ~Mixer() {}

	void step(void);
	void reset(void);
	int max_output(void) { return 1; }

private:
	const Desc		m_desc;
    int				m_type;
	int				m_size;
	int				m_r_node_bit_flag;
	int				m_c_bit_flag;
	double			m_r_total;
	const double *	m_r_node[DISC_MIXER_MAX_INPS];		/* Either pointer to resistance node output OR NULL */
	double			m_r_last[DISC_MIXER_MAX_INPS];
	double			m_exponent_rc[DISC_MIXER_MAX_INPS];	/* For high pass filtering cause by cIn */
	double			m_v_cap[DISC_MIXER_MAX_INPS];		/* cap voltage of each input */
	double			m_exponent_c_f;			/* Low pass on mixed inputs */
	double			m_exponent_c_amp;		/* Final high pass caused by out cap and amp input impedance */
	double			m_v_cap_f;				/* cap voltage of cF */
	double			m_v_cap_amp;			/* cap voltage of cAmp */
	double			m_gain;					/* used for DISC_MIXER_IS_OP_AMP_WITH_RI */
};

} // namespace Emulator