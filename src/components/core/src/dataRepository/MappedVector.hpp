/*
 * MapVectorContainer.hpp
 *
 *  Created on: Aug 23, 2017
 *      Author: settgast
 */

#ifndef SRC_COMPONENTS_CORE_SRC_DATAREPOSITORY_MAPVECTORCONTAINER_HPP_
#define SRC_COMPONENTS_CORE_SRC_DATAREPOSITORY_MAPVECTORCONTAINER_HPP_

#include "DataKey.hpp"
#include "SFINAE_Macros.hpp"

template< typename T, typename T_PTR=T*, typename KEY_TYPE=std::string, typename INDEX_TYPE = int >
class MappedVector
{
public:
  using key_type      = KEY_TYPE ;
  using mapped_type   = T_PTR;

  using LookupMapType          = std::unordered_map<KEY_TYPE, INDEX_TYPE >;
  using value_type             = typename std::pair< KEY_TYPE const, T_PTR >;
  using const_value_type       = typename std::pair< KEY_TYPE const, T const * >;
  using valueContainer         = std::vector<value_type>;
  using const_valueContainer   = std::vector<const_value_type>;
  using pointer                = typename valueContainer::pointer;
  using const_pointer          = typename valueContainer::const_pointer;
  using reference              = typename valueContainer::reference;
  using const_reference        = typename valueContainer::const_reference;
  using size_type              = typename valueContainer::size_type;


  using iterator               = typename valueContainer::iterator;
  using const_iterator         = typename const_valueContainer::const_iterator;
  using reverse_iterator       = typename valueContainer::reverse_iterator;
  using const_reverse_iterator = typename const_valueContainer::const_reverse_iterator;

  using DataKey = DataKeyT<KEY_TYPE const,INDEX_TYPE>;


  MappedVector() = default;
  ~MappedVector() = default;
  MappedVector( MappedVector const & source ) = default ;
  MappedVector & operator=( MappedVector const & source ) = default;

  MappedVector( MappedVector && source ) = default;
  MappedVector & operator=( MappedVector && source ) = default;


  T * insert( KEY_TYPE const & keyName, T_PTR source );


  /**
   * @name accessor functions
   */
  ///@{


  /**
   *
   * @param index
   * @return
   */
  inline T const * operator[]( INDEX_TYPE index ) const
  {
    return ( index>-1 && index<static_cast<INDEX_TYPE>( m_objects.size() ) ) ? const_cast<T const *>(&(*(m_objects[index].second))) : nullptr ;
  }

  /**
   *
   * @param index
   * @return
   */
  inline T * operator[]( INDEX_TYPE index )
  { return const_cast<T*>( const_cast< MappedVector<T,T_PTR,KEY_TYPE,INDEX_TYPE> const * >(this)->operator[](index) ); }

  /**
   *
   * @param keyName
   * @return
   */
  inline T const * operator[]( KEY_TYPE const & keyName ) const
  {
    typename LookupMapType::const_iterator iter = m_keyLookup.find(keyName);
    return ( iter!=m_keyLookup.end() ? this->operator[]( iter->second ) : nullptr );
  }

  /**
   *
   * @param keyName
   * @return
   */
  inline T * operator[]( KEY_TYPE const & keyName )
  { return const_cast<T*>( const_cast< MappedVector<T,T_PTR,KEY_TYPE,INDEX_TYPE> const * >(this)->operator[](keyName) ); }

  /**
   *
   * @param dataKey
   * @return
   */
  inline T const * operator[]( DataKey & dataKey ) const
  {
    INDEX_TYPE index = dataKey.Index();

    if( (index==-1) || (m_objects[index].first!=dataKey.Key()) )
    {
      index = getIndex( dataKey.Key() );
      dataKey.setIndex(index);
    }

    return this->operator[]( index );
  }

  /**
   *
   * @param dataKey
   * @return
   */
  inline T * operator[]( DataKey & dataKey )
  { return const_cast<T*>( const_cast< MappedVector<T,T_PTR,KEY_TYPE,INDEX_TYPE> const * >(this)->operator[](dataKey) ); }

  ///@}



  inline INDEX_TYPE getIndex( KEY_TYPE const & keyName ) const
  {
    typename LookupMapType::const_iterator iter = m_keyLookup.find(keyName);
    return ( iter!=m_keyLookup.end() ? iter->second : -1 );
  }


  void erase( INDEX_TYPE index )
  {
    m_objects[index] = nullptr;
    return;
  }

  void erase( KEY_TYPE const & keyName )
  {
    typename LookupMapType::const_iterator iter = m_keyLookup.find(keyName);
    if( iter!=m_keyLookup.end() )
    {
      m_objects[iter->second] = nullptr;
    }
    return;
  }

  void erase( DataKey & dataKey )
  {
    INDEX_TYPE index = dataKey.Index();

    if( (index==-1) || (m_objects[index].first!=dataKey.Key()) )
    {
      index = getIndex( dataKey.Key() );
      dataKey.setIndex(index);
    }
    erase( index );
  }

  void clear()
  {
    m_keyLookup.clear();
    m_objects.clear();
  }

  inline INDEX_TYPE size() const
  { return m_objects.size(); }

  inline valueContainer & values()
  { return m_objects; }

  inline const_valueContainer const & values() const
  { return this->m_constObjects; }

  inline LookupMapType const & keys() const
  { return m_keyLookup; }

  iterator begin()
  { return m_objects.begin(); }

  const_iterator begin() const
  { return m_constObjects.begin(); }

  iterator end()
  { return m_objects.end(); }

  const_iterator end() const
  { return m_constObjects.end(); }

private:
  valueContainer m_objects;
  const_valueContainer m_constObjects;

  LookupMapType m_keyLookup;
};

template< typename T, typename T_PTR, typename KEY_TYPE, typename INDEX_TYPE >
T * MappedVector<T,T_PTR,KEY_TYPE,INDEX_TYPE>::insert( KEY_TYPE const & keyName , T_PTR source )
{
  typename LookupMapType::iterator iterKeyLookup = m_keyLookup.find(keyName);

  INDEX_TYPE key = -1;
  // if the key was not found, make DataObject<T> and insert
  if( iterKeyLookup == m_keyLookup.end() )
  {
    value_type newEntry = std::make_pair( keyName, std::move( source ) );
    m_objects.push_back( std::move( newEntry ) );
    key = m_objects.size() - 1;

    m_keyLookup.insert( std::make_pair(keyName,key) );
    m_constObjects.push_back( std::make_pair( keyName, &(*(m_objects[key].second)) ) );

  }
  // if key was found, make sure it is empty
  else
  {
    key = iterKeyLookup->second;
    if( m_objects[key].second==nullptr )
    {
      m_objects[key].second = std::move( source );
      m_constObjects[key].second =  &(*(m_objects[key].second));
    }
    else
    {
      // error?
    }
  }

return &(*(m_objects[key].second));
}

#endif /* SRC_COMPONENTS_CORE_SRC_DATAREPOSITORY_MAPVECTORCONTAINER_HPP_ */